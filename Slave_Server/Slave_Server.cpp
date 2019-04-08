//
// Created by Сергей Ефимочкин on 11.11.2018.
//

#include "Slave_Server.h"
void Slave_Server::parse_answer_and_reply(const std::string &message) {

    if (message.find(std::string("ping")) == 0) {
    write_possible_sequence("received ping back");
    }
    else if (message.find(std::string("fs_info")) == 0) {
    std::string answer = "fs_info_back: n_blocks: " + std::to_string(number_of_blocks) + ", free_blocks: " + std::to_string(number_of_free_blocks);
    write_possible_sequence(answer);
    }
    else if (message.find(std::string("id")) == 0){
    sscanf(message.c_str(), "id: %d", &id);
    write_possible_sequence("started");
    }
    else if (message.find(std::string("command")) == 0) {
    parse_command_and_do_something(message);
    }
}

void Slave_Server::parse_command_and_do_something(const std::string &message){

    printf("received: %s\n", message.c_str());
    std::istringstream iss(message);
    std::string command;
    std::string first_arg;
    std::string second_arg;
    std::string trash;
    int client_id = -1;

    if (iss)
        iss >> trash;
    if (iss)
        iss >> command;
    if (iss)
        iss >> trash;
    if (iss)
        iss >> client_id;
    if (iss)
        iss >> trash;
    if (iss)
        iss >> first_arg;
    if (iss)
        iss >> trash;

    if (iss.tellg() > 0)
        second_arg = iss.str().substr(iss.tellg());
    else
        std::getline(iss, second_arg);
    if (second_arg.length() > 0)
        second_arg = second_arg.substr(1);

    if (command.find(std::string("send")) == 0){
        printf("YAAAAAAY, received: %s", message.c_str());
        write_possible_sequence(message);
    }

    if (command.find(std::string("to_store")) == 0){
        printf("storing: %s\n", second_arg.c_str());
        int index = fs.store_data_blocks(second_arg);
        int size_of_file_in_blocks = fs.get_size_of_data_in_FS_blocks(second_arg.length());
        std::string answer = "command: stored_whole id: " + std::to_string(client_id) + " first_arg: " + first_arg + " second_arg: " + std::to_string(index) + " " + std::to_string(size_of_file_in_blocks);
        write_possible_sequence(answer);
    }

    if (command.find(std::string("to_read")) == 0){
        std::istringstream siss(second_arg);
        int size_of_data = -1;
        std::string name_of_file;

        siss >> size_of_data;
        siss >> name_of_file;

        printf("reading: %s\n", message.c_str());
        std::string output = fs.read_data_blocks((unsigned int)std::stoi(first_arg), size_of_data);

        std::string answer = "command: read_whole id: " + std::to_string(client_id) + " first_arg: " + name_of_file + " second_arg: " + output;
        write_possible_sequence(answer);
    }

    if (command.find(std::string("to_free")) == 0){
        std::istringstream siss(second_arg);
        int size_of_data = -1;
        std::string name_of_file;

        siss >> size_of_data;
        siss >> name_of_file;

        printf("freeing: %s\n", name_of_file.c_str());

        fs.free_data_blocks((unsigned int)std::stoi(first_arg), (unsigned int)std::stoi(second_arg));

        std::string answer = "command: freed_whole id: " + std::to_string(client_id) + " first_arg: " + name_of_file + " second_arg: ";
        write_possible_sequence(answer);
    }
}