//
// Created by Сергей Ефимочкин on 11.11.2018.
//

#ifndef DFS_SLAVE_SERVER_H
#define DFS_SLAVE_SERVER_H
#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include "../utils/Server_Message.h"
#include "../Base_Classes/Server.h"
#include <string>
#include <fstream>




class Slave_Server : public Server
{
public:
    Slave_Server(boost::asio::io_service& io_service,
                tcp::resolver::iterator endpoint_iterator):
            Server(io_service, endpoint_iterator),
            io_service_(io_service),
            socket_(io_service)
    {
        parse_settings();
        initial_reserve_memory();
        reserved_memory = 0;
        Server_Message msg = Server_Message();
        msg.make_message("slave");
        write(msg);

    }

    ~Slave_Server() = default;


private:

    void parse_answer_and_reply(){
        std::string answer = std::string(read_msg_.body());


        if (answer.find(std::string("ping")) == 0) {
            Server_Message msg = Server_Message();
            msg.make_message("received ping back");
            write(msg);
        }
        else if (answer.find(std::string("start")) == 0) {
            write(Server_Message("started"));
        }
        else if (answer.find(std::string("overall size")) == 0) {
            char ans[20];
            snprintf(ans, 20, "overall size: %d", all_memory);
            write(Server_Message(ans));
        }


    }

    void parse_settings(){
        std::ifstream infile("../settings/slave_server_config.txt");
        char config_name[30];
        int value;
        while (infile >> config_name >> value)
        {
            if(strcmp(config_name, "memory_to_reserve") == 0)
                all_memory = value;
        }
    }

    void initial_reserve_memory(){}

    int get_size_of_free_memory(){
        return all_memory - reserved_memory;
    }

    void reserve_memory(int number_of_bytes);
    void free_memory(int number_of_bytes);



private:
    boost::asio::io_service& io_service_;
    tcp::socket socket_;
    Server_Message read_msg_;
    chat_message_queue write_msgs_;
    int all_memory;
    int reserved_memory;
};



#endif //DFS_SLAVE_SERVER_H


