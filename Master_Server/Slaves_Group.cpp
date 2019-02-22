//
// Created by Сергей Ефимочкин on 22.11.2018.
//

#include "Slaves_Group.h"
#include <sstream>


void Slaves_Group::parse(server_participant_ptr slave, std::string message){
    if (message.find(std::string("started")) == 0) {
        (*slave).write_possible_sequence("overall size");
    }


    else if (message.find(std::string("overall size")) == 0){
        int overall_memory_of_slave;
        sscanf(message.c_str(), "overall size: %d", &overall_memory_of_slave);
        sum_of_overall_memory += overall_memory_of_slave;
        slave_info server_info(slave, overall_memory_of_slave, 0);
        if (participants_.size() % 2 == 0){
            server_info.is_main = true;
        }
        else{
            for(auto info_it : slaves_info){
                if (info_it.second.is_main && info_it.second.backup_slave == nullptr){
                    info_it.second.backup_slave = slave;
                    server_info.is_backup = true;
                    server_info.server_to_backup = info_it.first;
                    break;
                }
            }
        }
        slaves_info.insert({slave, server_info});
    }
    else if (message.find(std::string("command")) == 0) {
        parse_command_and_do_something(message);
    }
}

void Slaves_Group::parse_command_and_do_something(std::string message){
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

    if (command.find(std::string("send")) == 0)
        other_group_->send_command(message);
}


void Slaves_Group::send_command(std::string message) {
    int max_free_size = 0;
    server_participant_ptr winner;

    for (auto &it: slaves_info) {
        if (it.second.overall_memory > max_free_size)
            winner = it.first;
    }
    (*winner).write_possible_sequence(message);
}
