//
// Created by Сергей Ефимочкин on 2019-01-28.
//

#ifndef DFS_SLAVE_CLIENT_SERVER_H
#define DFS_SLAVE_CLIENT_SERVER_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "../utils/Server_Message.h"
#include <fstream>
#include <string>

using boost::asio::ip::tcp;

class Client_Server
{
public:
    Client_Server(boost::asio::io_service& io_service,
                tcp::resolver::iterator endpoint_iterator)
            : io_service_(io_service),
              socket_(io_service)
    {
        do_connect(endpoint_iterator);
        Server_Message msg = Server_Message();
        msg.make_message("client");
        write(msg);
    }

    void write(const Server_Message& msg)
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

    void close()
    {
        io_service_.post([this]() { socket_.close(); });
    }

private:
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
                                    if (!ec && read_msg_.decode_header())
                                    {

                                        do_read_body();

                                    }
                                    else
                                    {
                                        socket_.close();
                                    }
                                });
    }

    void do_read_body()
    {
        boost::asio::async_read(socket_,
                                boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                                [this](boost::system::error_code ec, std::size_t /*length*/)
                                {
                                    if (!ec)
                                    {
                                        std::cout.write(read_msg_.body(), read_msg_.body_length());

                                        std::cout << "\n";
                                        parse_answer_and_reply();

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

    void parse_answer_and_reply(){
        std::string answer = std::string(read_msg_.body());


        if (answer.find(std::string("ping")) == 0) {
            Server_Message msg = Server_Message();
            msg.make_message("received ping back");
            write(msg);
        }
        else if (answer.find(std::string("start")) == 0) {
            write(Server_Message("client"));
        }


    }




private:
    boost::asio::io_service& io_service_;
    tcp::socket socket_;
    Server_Message read_msg_;
    chat_message_queue write_msgs_;
};



#endif //DFS_SLAVE_CLIENT_SERVER_H
