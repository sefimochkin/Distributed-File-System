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
#include "Slave_FS_Handler.h"




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

    void parse_command_and_do_something(const std::string &message){
        std::istringstream iss(message);
        std::string command;
        std::string first_arg;
        std::string second_arg;
        std::string trash;
        int id = -1;

        if (iss)
            iss >> trash;
        if (iss)
            iss >> command;
        if (iss)
            iss >> trash;
        if (iss)
            iss >> id;
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

        if (command.find(std::string("store_whole")) == 0){
            printf("storing: %s", first_arg.c_str());
            int index = fs.store_data_blocks(first_arg);
            std::string answer = "command: store_whole id: " + std::to_string(id) + " first_arg: " + std::to_string(index);
            write_possible_sequence(answer);
        }

        if (command.find(std::string("read_whole")) == 0){
            printf("reading: %s", message.c_str());
            std::string output = fs.read_data_blocks((unsigned int)std::stoi(first_arg), (unsigned int)std::stoi(second_arg));

            std::string answer = "command: read_whole id: " + std::to_string(id) + " first_arg: _ second_arg: " + output;
            write_possible_sequence(answer);
        }

        if (command.find(std::string("free_whole")) == 0){
            printf("reading: %s", message.c_str());
            fs.free_data_blocks((unsigned int)std::stoi(first_arg), (unsigned int)std::stoi(second_arg));

            std::string answer = "command: freed_whole id: " + std::to_string(id) + " first_arg: _ second_arg: ";
            write_possible_sequence(answer);
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
    Slave_FS_Handler fs;
};



#endif //DFS_SLAVE_SERVER_H


