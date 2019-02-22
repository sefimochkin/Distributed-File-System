//
// Created by Сергей Ефимочкин on 21.11.2018.
//

#ifndef DFS_INTER_SERVER_SESSION_H
#define DFS_INTER_SERVER_SESSION_H
#include <boost/asio.hpp>
#include <deque>
#include "Server_Group.h"
#include "../utils/Server_Message.h"
#include <mutex>

using boost::asio::ip::tcp;

class Server_Group;

class Server_Participant
{
public:
    virtual ~Server_Participant() {}
    virtual void deliver(const Server_Message& msg) = 0;
    virtual void write_possible_sequence(const std::string& message) = 0;
    virtual void write_possible_sequence(const char * msg) = 0;
    virtual Server_Message get_message() = 0;
    int id;
};

class Inter_Server_Session: public Server_Participant,
                            public std::enable_shared_from_this<Inter_Server_Session>
{
public:
    Inter_Server_Session(tcp::socket socket, Server_Group& group)
            : socket_(std::move(socket)), group_(group){
    }

    void start();

    void deliver(const Server_Message& msg);

    void write_possible_sequence(const std::string& message);

    void write_possible_sequence(const char * msg);


protected:
    void do_read_header();

    virtual void do_read_body(int header_code);

    void do_write();

protected:

    std::mutex write_mutex;
    Server_Message get_message();
    Server_Group& group_;
    tcp::socket socket_;
    Server_Message read_msg_;
    chat_message_queue write_msgs_;
};

#endif //DFS_INTER_SERVER_SESSION_H
