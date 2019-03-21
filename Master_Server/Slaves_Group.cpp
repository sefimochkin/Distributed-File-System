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

        int slave_id = -1;
        for(const auto &participant_it : participants_){
            if (participant_it.second == slave){
                slave_id = participant_it.first;
                break;
            }
        }

        slave_info server_info(slave, overall_memory_of_slave, 0, slave_id);
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
    std::getline(iss, second_arg);
    if (second_arg.length() > 0)
        second_arg = second_arg.substr(1);

    if (command.find(std::string("send")) == 0)
        other_group_->send_command(message);

    else if (command.find(std::string("stored_whole")) == 0){
        files[first_arg].index_of_file = std::stoi(second_arg);
        files[first_arg].rw_mtx->unlock();
    }

    else if (command.find(std::string("read_whole")) == 0) {
        files[first_arg].rw_mtx->unlock_shared();
        other_group_->send_command(message);
    }

    else if (command.find(std::string("freed_whole")) == 0){
        files[first_arg].rw_mtx->unlock();
    }
}




void Slaves_Group::send_command(std::string message) {
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
    std::getline(iss, second_arg);
    if (second_arg.length() > 0)
        second_arg = second_arg.substr(1);

    if (command.find(std::string("to_store")) == 0){

        int max_free_size = 0;
        server_participant_ptr winner;
        int winner_slave_id = -1;

        for (auto &it: slaves_info) {
            if (it.second.overall_memory > max_free_size) {
                winner = it.first;
                winner_slave_id = it.second.id;
            }
        }
        printf("making stuff\n");
        //storing partial information about data in slave, the data's location in slave will be filled out when
        //the slave sends it back
        files[first_arg] = file_bindings(winner_slave_id, second_arg.length());
        printf("locking stuff\n");
        files[first_arg].rw_mtx->lock();
        printf("sending stuff\n");
        (*winner).write_possible_sequence(message);
    }

    else if (command.find(std::string("to_read")) == 0){
        files[first_arg].rw_mtx->lock_shared();
        int index_of_data = files[first_arg].index_of_file;
        int size_of_data = files[first_arg].size_of_data;
        int slave_id = files[first_arg].storage_slave_id;
        std::string command_to_slave = "command: to_read id: " + std::to_string(client_id) + " first_arg: " + std::to_string(index_of_data) + " second_arg: " + std::to_string(size_of_data) + " " + first_arg;
        participants_[slave_id]->write_possible_sequence(command_to_slave);
    }

    else if (command.find(std::string("to_free")) == 0){
        files[first_arg].rw_mtx->lock();
        int index_of_data = files[first_arg].index_of_file;
        int size_of_data = files[first_arg].size_of_data;
        int slave_id = files[first_arg].storage_slave_id;
        std::string command_to_slave = "command: to_free id: " + std::to_string(client_id) + " first_arg: " + std::to_string(index_of_data) + " second_arg: " + std::to_string(size_of_data) + " " + first_arg;
        participants_[slave_id]->write_possible_sequence(command_to_slave);
    }
}

void Slaves_Group::do_something_on_leave(int server_id) {}
