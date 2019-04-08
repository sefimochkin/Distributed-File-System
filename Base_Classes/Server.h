//
// Created by Сергей Ефимочкин on 2019-01-30.
//

#ifndef DFS_BASE_SERVER_H
#define DFS_BASE_SERVER_H

#include <boost/asio.hpp>
#include "../utils/Server_Message.h"
#include <deque>
#include <iostream>


using boost::asio::ip::tcp;

class Server
{
public:
    Server(boost::asio::io_service& io_service,
                  tcp::resolver::iterator endpoint_iterator)
            : io_service_(io_service),
              socket_(io_service)
    {
        do_connect(endpoint_iterator);
    }

    //Server(){}

    virtual void write(const Server_Message& msg)
    {
        io_service_.post(
                [this, msg]()
                {
                    bool write_in_progress = !write_msgs_.empty();
                    write_msgs_.push_back(msg);
                    if (!write_in_progress)
                    {
                        do_write();
                    }
                });
    }

    void write_possible_sequence(const std::string& message){
        io_service_.post(
                [this, message]()
                {   //write(Server_Message("id: " + std::to_string(id), false));
                    std::queue<Server_Message> msgs = Server_Message::make_messages(message);
                    while (!msgs.empty()){
                        write(msgs.front());
                        msgs.pop();
                    }
                });
    }

    void write_possible_sequence(const char * msg){
        write_possible_sequence(std::string(msg));
    }

    void close()
    {
        io_service_.post([this]() { socket_.close(); });
    }

    virtual ~Server() = default;


protected:
    void do_connect(tcp::resolver::iterator endpoint_iterator)
    {
        boost::asio::async_connect(socket_, endpoint_iterator,
                                   [this](boost::system::error_code ec, tcp::resolver::iterator)
                                   {
                                       if (!ec)
                                       {
                                           do_read_header();
                                       }
                                   });
    }

    void do_read_header()
    {
        boost::asio::async_read(socket_,
                                boost::asio::buffer(read_msg_.data(), Server_Message::header_length),
                                [this](boost::system::error_code ec, std::size_t /*length*/)
                                {
                                    int header_code = read_msg_.decode_header();
                                    if (!ec && (header_code != 0))
                                    {
                                        do_read_body(header_code);
                                    }
                                    else
                                    {
                                        socket_.close();
                                    }
                                });
    }

    void do_read_body(int header_code)
    {
        boost::asio::async_read(socket_,
                                boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                                [this, header_code](boost::system::error_code ec, std::size_t /*length*/)
                                {
                                    if (!ec)
                                    {
                                        parse_possible_sequence(std::string(read_msg_.read()), header_code);

                                        do_read_header();
                                    }
                                    else
                                    {
                                        socket_.close();
                                    }
                                });
    }

    void do_write()
    {
        boost::asio::async_write(socket_,
                                 boost::asio::buffer(write_msgs_.front().data(),
                                                     write_msgs_.front().length()),
                                 [this](boost::system::error_code ec, std::size_t /*length*/)
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
                                         socket_.close();
                                     }
                                 });
    }

    void parse_possible_sequence(const std::string& message, int header_code){
        if (header_code == 2) {
            if (strcmp(continuous_message.c_str(), "") == 0) {
                //printf("%s\n", message.c_str());
                parse_answer_and_reply(message);

            } else {
                continuous_message += message;
                //printf("%s\n", continuous_message.c_str());
                parse_answer_and_reply(continuous_message);
                continuous_message = "";
            }
        }
        else{
            continuous_message += message;
        }
    }

    virtual void parse_answer_and_reply(const std::string &message) = 0;

protected:
    int id;
    boost::asio::io_service& io_service_;
    tcp::socket socket_;
    Server_Message read_msg_;
    chat_message_queue write_msgs_;
    std::string continuous_message;
};



#endif //DFS_BASE_SERVER_H
