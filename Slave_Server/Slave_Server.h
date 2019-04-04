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
            Server(io_service, endpoint_iterator), fs(Slave_FS_Handler(number_of_blocks, number_of_free_blocks))
    {
        write_possible_sequence("slave");
    }

    ~Slave_Server() = default;


private:

    void parse_answer_and_reply(const std::string &message) override;

    void parse_command_and_do_something(const std::string &message);


    /*
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
    */


private:
    int number_of_blocks = 16384;
    int number_of_free_blocks = 16384;
    Slave_FS_Handler fs;
};



#endif //DFS_SLAVE_SERVER_H


