//
// Created by Сергей Ефимочкин on 2019-02-19.
//

#include "Clients_Group.h"

void Clients_Group::parse(server_participant_ptr slave, std::string message){
    if (message.find(std::string("started")) == 0) {
        (*slave).write_possible_sequence("overall size");
    }

    if (message.find(std::string("send")) == 0) {
        slaves_group->send_command(slave, message);
    }
}