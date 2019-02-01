//
// Created by Сергей Ефимочкин on 22.11.2018.
//

#include "Slaves_Group.h"

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
}