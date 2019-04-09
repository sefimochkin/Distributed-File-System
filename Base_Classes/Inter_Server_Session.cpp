//
// Created by Сергей Ефимочкин on 21.11.2018.
//
#include "Inter_Server_Session.h"
#include <boost/asio.hpp>



void Inter_Server_Session::start()
{
    group_.join(shared_from_this());
    do_read_header();
}

void Inter_Server_Session::deliver(const Server_Message& msg)
{
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
        do_write();
    }
}

void Inter_Server_Session::write_possible_sequence(const std::string& message){
    //write(Server_Message("id: " + std::to_string(id), false));
    write_mutex.lock();
    std::queue<Server_Message> msgs = Server_Message::make_messages(message);
    while (!msgs.empty()){
        deliver(msgs.front());
        msgs.pop();
    }
    write_mutex.unlock();


}

void Inter_Server_Session::write_possible_sequence(const char * msg){
    write_possible_sequence(std::string(msg));
}

void Inter_Server_Session::do_read_header()
{
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
                            boost::asio::buffer(read_msg_.data(), Server_Message::header_length),
                            [this, self](boost::system::error_code ec, std::size_t /*length*/)
                            {
                                int header_code = read_msg_.decode_header();
                                if (!ec && (header_code != 0))
                                {
                                    do_read_body(header_code);
                                }
                                else
                                {
                                    group_.leave(shared_from_this());
                                }
                            });

}

void Inter_Server_Session::do_read_body(int header_code)
{
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
                            boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                            [this, self, header_code](boost::system::error_code ec, std::size_t /*length*/)
                            {
                                if (!ec)
                                {
                                    group_.parse_possible_sequence(shared_from_this(), std::string(read_msg_.read()), header_code);

                                    do_read_header();
                                }
                                else
                                {
                                    group_.leave(shared_from_this());
                                }
                            });
}

void Inter_Server_Session::do_write()
{
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
                             boost::asio::buffer(write_msgs_.front().data(),
                                                 write_msgs_.front().length()),
                             [this, self](boost::system::error_code ec, std::size_t /*length*/)
                             {
                                 if (!ec)
                                 {
                                     write_msgs_.pop_front();
                                     if (!write_msgs_.empty())
                                     {
                                         do_write();
                                     }
                                 }
                                 else
                                 {
                                     group_.leave(shared_from_this());
                                 }
                             });
}

Server_Message Inter_Server_Session::get_message(){
    do_read_header();
    return read_msg_;
}
