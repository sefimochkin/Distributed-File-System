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
#include <sstream>




class Slave_Server : public Server
{
public:
    Slave_Server(boost::asio::io_service& io_service,
                tcp::resolver::iterator endpoint_iterator):
            Server(io_service, endpoint_iterator)
    {
        parse_settings();
        initial_reserve_memory();
        reserved_memory = 0;

        write_possible_sequence("slave");
    }

    ~Slave_Server() = default;


private:

    void parse_answer_and_reply(const std::string &message) override {

        if (message.find(std::string("ping")) == 0) {
            write_possible_sequence("received ping back");
        }
        else if (message.find(std::string("overall size")) == 0) {
            char ans[20];
            snprintf(ans, 20, "overall size: %d", all_memory);
            write_possible_sequence(ans);
        }
        else if (message.find(std::string("id")) == 0){
            sscanf(message.c_str(), "id: %d", &id);
            write_possible_sequence("started");
        }
        else if (message.find(std::string("command")) == 0) {
            parse_command_and_do_something(message);
        }
    }

    void parse_command_and_do_something(std::string message){
        std::istringstream iss(message);
        std::string command;
        std::string first_arg;
        std::string second_arg;
        std::string trash;

        if (iss)
            iss >> trash;
        if (iss)
            iss >> command;
        if (iss)
            iss >> trash;
        if (iss)
            iss >> trash;
        if (iss)
            iss >> trash;
        if (iss)
            iss >> first_arg;
        if (iss)
            iss >> trash;
        std::getline(iss, second_arg);

        if (command.find(std::string("send")) == 0){
            printf("YAAAAAAY, received: %s", message.c_str());
            write_possible_sequence(message);
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
    int all_memory;
    int reserved_memory;
};



#endif //DFS_SLAVE_SERVER_H


