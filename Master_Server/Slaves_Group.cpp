//
// Created by Сергей Ефимочкин on 22.11.2018.
//

#include "Slaves_Group.h"
#include <sstream>


void Slaves_Group::parse(server_participant_ptr slave, std::string message){
    if (message.find(std::string("started")) == 0) {
        (*slave).write_possible_sequence("fs_info");
    }


    else if (message.find(std::string("fs_info_back")) == 0){
        int n_blocks, free_blocks;
        sscanf(message.c_str(), "fs_info_back: n_blocks: %d, free_blocks: %d", &n_blocks, &free_blocks);

        counter_mutex->lock();
        n_of_all_blocks += n_blocks;
        all_free_blocks += free_blocks;
        counter_mutex->unlock();



        int slave_id = -1;
        for(const auto &participant_it : participants_){
            if (participant_it.second == slave){
                slave_id = participant_it.first;
                break;
            }
        }

        slave_info server_info(slave, n_blocks, free_blocks, slave_id);
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
        std::istringstream iss_stored(second_arg);
        std::string index_of_file;
        std::string size_in_blocks;

        if (iss_stored)
            iss_stored >> index_of_file;
        if (iss_stored)
            iss_stored >> size_in_blocks;
        int inode_id = std::stoi(first_arg);


        files[inode_id].index_of_file = std::stoi(index_of_file);
        files[inode_id].size_of_file_in_blocks = std::stoi(size_in_blocks);
        files[inode_id].rw_mtx->unlock();

        counter_mutex->lock();
        all_free_blocks -= files[inode_id].size_of_file_in_blocks;
        counter_mutex->unlock();

        for (auto &it: slaves_info) {
            if (it.second.id == files[inode_id].storage_slave_id) {
                it.second.counter_mutex->lock();
                it.second.free_blocks -= std::stoi(size_in_blocks);
                it.second.counter_mutex->unlock();
                break;
            }
        }

    }

    else if (command.find(std::string("read_whole")) == 0) {
        int inode_id = std::stoi(first_arg);
        files[inode_id].rw_mtx->unlock_shared();
        other_group_->send_command(message);
    }

    else if (command.find(std::string("freed_whole")) == 0){
        int inode_id = std::stoi(first_arg);
        files[inode_id].rw_mtx->unlock();

        counter_mutex->lock();
        all_free_blocks += files[inode_id].size_of_file_in_blocks;
        counter_mutex->unlock();

        for (auto &it: slaves_info) {
            if (it.second.id == files[inode_id].storage_slave_id) {
                it.second.counter_mutex->lock();
                it.second.free_blocks += files[inode_id].size_of_file_in_blocks;
                it.second.counter_mutex->unlock();
                files.erase(inode_id);
                break;
            }
        }
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
        int inode_id = std::stoi(first_arg);
        int max_free_size = 0;
        server_participant_ptr winner;
        int winner_slave_id = -1;

        for (auto &it: slaves_info) {
            if (it.second.free_blocks > max_free_size) {
                winner = it.first;
                winner_slave_id = it.second.id;

                it.second.free_blocks -= 1;
            }
        }

        printf("making stuff\n");
        //storing partial information about data in slave, the data's location in slave will be filled out when
        //the slave sends it back
        files[inode_id] = file_bindings(winner_slave_id, second_arg.length());
        printf("locking stuff\n");
        files[inode_id].rw_mtx->lock();
        printf("sending stuff\n");
        (*winner).write_possible_sequence(message);
    }

    else if (command.find(std::string("to_read")) == 0){
        int inode_id = std::stoi(first_arg);
        files[inode_id].rw_mtx->lock_shared();
        int index_of_data = files[inode_id].index_of_file;
        int size_of_data = files[inode_id].size_of_data;
        int slave_id = files[inode_id].storage_slave_id;
        std::string command_to_slave = "command: to_read id: " + std::to_string(client_id) + " first_arg: " + std::to_string(index_of_data) + " second_arg: " + std::to_string(size_of_data) + " " + first_arg;
        participants_[slave_id]->write_possible_sequence(command_to_slave);
    }

    else if (command.find(std::string("to_free")) == 0){
        int inode_id = std::stoi(first_arg);
        files[inode_id].rw_mtx->lock();
        int index_of_data = files[inode_id].index_of_file;
        int size_of_data = files[inode_id].size_of_data;
        int slave_id = files[inode_id].storage_slave_id;
        std::string command_to_slave = "command: to_free id: " + std::to_string(client_id) + " first_arg: " + std::to_string(index_of_data) + " second_arg: " + std::to_string(size_of_data) + " " + first_arg;
        participants_[slave_id]->write_possible_sequence(command_to_slave);
    }
}

std::string Slaves_Group::get_fs_info(){
    counter_mutex->lock();
    std::string answer = "n_blocks: " + std::to_string(n_of_all_blocks) + ", free_blocks: " + std::to_string(all_free_blocks);
    counter_mutex->unlock();
    return answer;
}

int Slaves_Group::count_max_size_of_data_by_number_of_blocks(int n_blocks) {
    return n_blocks * NUMBER_OF_BYTES_IN_BLOCK / (NUMBER_OF_CHARS_IN_INDEX + 1);
}


void Slaves_Group::do_something_on_leave(int server_id) {}
