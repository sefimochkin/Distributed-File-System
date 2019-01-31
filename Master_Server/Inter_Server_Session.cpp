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

void Inter_Server_Session::do_read_header()
{
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
                            boost::asio::buffer(read_msg_.data(), Server_Message::header_length),
                            [this, self](boost::system::error_code ec, std::size_t /*length*/)
                            {
                                if (!ec && read_msg_.decode_header())
                                {
                                    do_read_body();
                                }
                                else
                                {
                                    group_.leave(shared_from_this());
                                }
                            });

}

void Inter_Server_Session::do_read_body()
{
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
                            boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                            [this, self](boost::system::error_code ec, std::size_t /*length*/)
                            {
                                if (!ec)
                                {
                                    printf("%s\n", read_msg_.body());
                                    group_.parse(shared_from_this(), read_msg_.body());

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
