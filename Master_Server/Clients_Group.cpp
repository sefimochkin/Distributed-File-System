//
// Created by Сергей Ефимочкин on 2019-02-19.
//

#include "Clients_Group.h"
#include <sstream>

void Clients_Group::parse(server_participant_ptr client, std::string message){
    if (message.find(std::string("started")) == 0) {
        (*client).write_possible_sequence("overall size");
    }

    if (message.find(std::string("command")) == 0) {
        parse_command_and_do_something(message);
    }
}

    void Clients_Group::parse_command_and_do_something(std::string message){
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
            iss >> trash;
        if (iss)
            iss >> first_arg;
        if (iss)
            iss >> trash;
        std::getline(iss, second_arg);

        //printf("got command: %s\n", command.c_str());

        if (message.find(std::string("send")) == 0) {
            printf("sending to slaves_group!\n");
            other_group_->send_command(message);
        }

        else {
            std::string answer = "command: print, id: " + std::to_string(id) + ", first_arg: " + fs.do_command(id, command, first_arg, second_arg) + " second_arg: ";
            participants_[id]->write_possible_sequence(answer);
        }
    }

void Clients_Group::send_command(std::string message) {
    int id = -1;

    std::istringstream iss(message);
    std::string command;
    //std::string id_str;
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
        iss >> id;
    if (iss)
        iss >> trash;
    if (iss)
        iss >> first_arg;
    if (iss)
        iss >> trash;
    std::getline(iss, second_arg);

    if (command.find(std::string("send")) == 0) {
        participants_[id]->write_possible_sequence(message);
    }
    //(*winner).write_possible_sequence(message);
}