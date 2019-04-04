//
// Created by Сергей Ефимочкин on 22.11.2018.
//

#include "Slaves_Group.h"
#include <sstream>
#include <random>
#include <stack>



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
        std::istringstream sarg_iss(second_arg);
        int index_of_file;
        int slave_id;
        sarg_iss >> index_of_file;
        sarg_iss >> slave_id;
        int inode_id = std::stoi(first_arg);

        file_bindings * possible_file_binding_part = &files[inode_id];
        std::stack <file_bindings*> reverse_seq_of_parts;
        while (possible_file_binding_part->next_part != nullptr || possible_file_binding_part->storage_slave_id != slave_id) {
            reverse_seq_of_parts.push(possible_file_binding_part);
            possible_file_binding_part = possible_file_binding_part->next_part.get();
        }

        possible_file_binding_part->n_of_further_parts_waiting_for_answer--;
        possible_file_binding_part->index_of_file = index_of_file;
        if(possible_file_binding_part->n_of_further_parts_waiting_for_answer == 0)
            possible_file_binding_part->rw_mtx->unlock();

        while (!reverse_seq_of_parts.empty()){
            reverse_seq_of_parts.top()->n_of_further_parts_waiting_for_answer--;
            if(reverse_seq_of_parts.top()->n_of_further_parts_waiting_for_answer == 0)
                reverse_seq_of_parts.top()->rw_mtx->unlock();
            reverse_seq_of_parts.pop();
        }
    }

    else if (command.find(std::string("read_whole")) == 0) {
        std::istringstream sarg_iss(second_arg);
        int slave_id = -1;
        int i_in_constructor = -1;
        sarg_iss >> slave_id;
        sarg_iss >> i_in_constructor;

        std::getline(sarg_iss, second_arg);
        if (second_arg.length() > 0)
            second_arg = second_arg.substr(1);

        int inode_id = std::stoi(first_arg);

        if (files[inode_id].size_in_parts == 1) {
            std::string new_message = "command: read_whole id: " + std::to_string(client_id) + " first_arg: " + std::to_string(inode_id) + " second_arg: " + second_arg;
            files[inode_id].rw_mtx->unlock_shared();
            other_group_->send_command(new_message);
        }

        else {
            file_bindings *possible_file_binding_part = &files[inode_id];
            std::stack<file_bindings *> reverse_seq_of_parts;
            while (possible_file_binding_part->next_part != nullptr ||
                   possible_file_binding_part->storage_slave_id != slave_id) {
                reverse_seq_of_parts.push(possible_file_binding_part);
                possible_file_binding_part = possible_file_binding_part->next_part.get();
            }
            possible_file_binding_part->n_of_further_parts_waiting_for_answer_for_read[client_id].first->lock();
            possible_file_binding_part->n_of_further_parts_waiting_for_answer_for_read[client_id].second--;
            possible_file_binding_part->n_of_further_parts_waiting_for_answer_for_read[client_id].first->unlock();

            file_constructors[client_id][inode_id].mtx->lock();
            file_constructors[client_id][inode_id].n_of_further_parts_waiting_for_answer--;
            file_constructors[client_id][inode_id].parts_of_string[i_in_constructor] = &second_arg;
            file_constructors[client_id][inode_id].mtx->unlock();

            possible_file_binding_part->n_of_further_parts_waiting_for_answer_for_read[client_id].first->lock();
            if (possible_file_binding_part->n_of_further_parts_waiting_for_answer_for_read[client_id].second == 0)
                possible_file_binding_part->rw_mtx->unlock_shared();
            possible_file_binding_part->n_of_further_parts_waiting_for_answer_for_read[client_id].first->unlock();

            while (!reverse_seq_of_parts.empty()) {
                reverse_seq_of_parts.top()->n_of_further_parts_waiting_for_answer_for_read[client_id].first->lock();
                reverse_seq_of_parts.top()->n_of_further_parts_waiting_for_answer_for_read[client_id].second--;
                if (reverse_seq_of_parts.top()->n_of_further_parts_waiting_for_answer == 0)
                    reverse_seq_of_parts.top()->rw_mtx->unlock_shared();
                reverse_seq_of_parts.top()->n_of_further_parts_waiting_for_answer_for_read[client_id].first->unlock();
                reverse_seq_of_parts.pop();
            }

            file_constructors[client_id][inode_id].mtx->lock();
            if (file_constructors[client_id][inode_id].n_of_further_parts_waiting_for_answer == 0) {
                std::string constructed_string;
                for (auto it : file_constructors[client_id][inode_id].parts_of_string) {
                    constructed_string += *it;
                }
                std::string new_message = "command: read_whole id: " + std::to_string(client_id) + " first_arg: " +
                                          std::to_string(inode_id) + " second_arg: " + constructed_string;
                other_group_->send_command(new_message);
                file_constructors[client_id][inode_id].mtx->unlock();
                file_constructors[client_id].erase(inode_id);
            } else {
                file_constructors[client_id][inode_id].mtx->unlock();
            }
        }
    }

    else if (command.find(std::string("freed_whole")) == 0){
        int slave_id = std::stoi(second_arg);
        int inode_id = std::stoi(first_arg);

        file_bindings * possible_file_binding_part = &files[inode_id];
        std::stack <file_bindings*> reverse_seq_of_parts;
        while (possible_file_binding_part->next_part != nullptr || possible_file_binding_part->storage_slave_id != slave_id) {
            reverse_seq_of_parts.push(possible_file_binding_part);
            possible_file_binding_part = possible_file_binding_part->next_part.get();
        }

        possible_file_binding_part->n_of_further_parts_waiting_for_answer--;

        int size_of_file_in_blocks = get_size_of_data_in_fs_blocks(files[inode_id].size_of_data);

        counter_mutex->lock();
        all_free_blocks += size_of_file_in_blocks;

        for (auto &it: slaves_info) {
            if (it.second.id == files[inode_id].storage_slave_id) {
                it.second.counter_mutex->lock();
                it.second.free_blocks += size_of_file_in_blocks;
                it.second.counter_mutex->unlock();
                break;
            }
        }
        counter_mutex->unlock();

        if(possible_file_binding_part->n_of_further_parts_waiting_for_answer == 0)
            possible_file_binding_part->rw_mtx->unlock();

        while (!reverse_seq_of_parts.empty()){
            reverse_seq_of_parts.top()->n_of_further_parts_waiting_for_answer--;

            if(reverse_seq_of_parts.top()->n_of_further_parts_waiting_for_answer == 0) {
                if (reverse_seq_of_parts.top() == &files[inode_id]){
                    reverse_seq_of_parts.top()->rw_mtx->unlock();
                    files.erase(inode_id);
                }
                else
                    reverse_seq_of_parts.top()->rw_mtx->unlock();
            }
            reverse_seq_of_parts.pop();
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

        std::vector<std::pair<server_participant_ptr, slave_info> > sorted_vector(
                slaves_info.begin(), slaves_info.end());
        sort(sorted_vector.begin(), sorted_vector.end(), inverse_slave_info_map_comparator());

        int index_of_last_ok_slave = -1;

        int number_of_blocks_needed = get_size_of_data_in_fs_blocks(second_arg.length());

        auto it = sorted_vector.begin();
        for(; it != sorted_vector.end(); it++){
            index_of_last_ok_slave++;
            if ((*it).second.free_blocks < number_of_blocks_needed) {
                index_of_last_ok_slave--;
                break;
            }
        }

        // THE CODE FURTHER FUCKS EVERYTHING UP! IT LEADS TO TWO THREADS USING SAME CLIENTS GROUP AND IT SOMEHOW KILLS EVERYTHING. SORRY, WILL FIGURE IT OUT LATER

        if(index_of_last_ok_slave > -1) {
            int index_of_winner =  (rand() % static_cast<int>(index_of_last_ok_slave + 1));
            server_participant_ptr winner = sorted_vector[index_of_winner].first;

            //storing partial information about data in slave, the data's location in slave will be filled out when
            //the slave sends it back
            files[inode_id] = file_bindings(sorted_vector[index_of_winner].second.id, second_arg.length());
            files[inode_id].n_of_further_parts_waiting_for_answer = 1;
            files[inode_id].size_in_parts = 1;
            send_data_to_slave(inode_id, winner, message, files[inode_id]);
        }

        else {
            int chars_left_to_store = second_arg.length();
            int i_slave = 0;
            int start_of_substring = 0;
            file_bindings * last_file_bind = nullptr;

            int parts_unstored = 0;

            while(chars_left_to_store > 0){
                chars_left_to_store -= count_max_size_of_data_by_number_of_blocks(sorted_vector[i_slave].second.free_blocks);
                parts_unstored++;
            }

            chars_left_to_store = second_arg.length();

            while (chars_left_to_store > 0) {
                int chars_to_store = count_max_size_of_data_by_number_of_blocks(sorted_vector[i_slave].second.free_blocks);
                std::string substring_to_store = second_arg.substr(start_of_substring, chars_to_store);
                std::string sub_message = "command: to_store id: " + std::to_string(client_id) + " first_arg: " + std::to_string(inode_id) + " second_arg: " + substring_to_store;
                file_bindings file_binding_part = file_bindings(sorted_vector[i_slave].second.id, substring_to_store.length());
                file_binding_part.n_of_further_parts_waiting_for_answer = parts_unstored;


                if (last_file_bind != nullptr) {
                    last_file_bind->next_part.reset(&file_binding_part);
                }
                else {
                    files[inode_id] = file_binding_part;
                    files[inode_id].size_in_parts = parts_unstored;
                }

                send_data_to_slave(inode_id, sorted_vector[i_slave].first, sub_message, file_binding_part);
                last_file_bind = &file_binding_part;
                i_slave++;
                start_of_substring += chars_to_store;
                chars_left_to_store -= chars_to_store;
                parts_unstored--;
            }
        }
    }

    else if (command.find(std::string("to_read")) == 0){
        int inode_id = std::stoi(first_arg);

        if (files[inode_id].size_in_parts == 1) {
            files[inode_id].rw_mtx->lock_shared();
            std::string command_to_slave = "command: to_read id: " + std::to_string(client_id) + " first_arg: " + std::to_string(files[inode_id].index_of_file) + " second_arg: " + std::to_string(files[inode_id].size_of_data) + " -1 " + first_arg;
            participants_[files[inode_id].storage_slave_id]->write_possible_sequence(command_to_slave);
        }

        else {
            file_bindings * current_file_bind = &files[inode_id];
            int fb_i = 0;

            file_constructors[client_id][inode_id] = read_file_constructor(files[inode_id].size_in_parts);

            do {
                files[inode_id].rw_mtx->lock_shared();
                current_file_bind->n_of_further_parts_waiting_for_answer_for_read.emplace(client_id, std::pair< std::shared_ptr< boost::mutex >, int >(new boost::mutex(), files[inode_id].size_in_parts - fb_i));

                std::string command_to_slave = "command: to_read id: " + std::to_string(client_id) + " first_arg: " + std::to_string(files[inode_id].index_of_file) + " second_arg: " + std::to_string(files[inode_id].size_of_data) + " " + std::to_string(fb_i) + " " + first_arg;
                participants_[current_file_bind->storage_slave_id]->write_possible_sequence(command_to_slave);

                current_file_bind = current_file_bind->next_part.get();
                fb_i++;
            }
            while(current_file_bind != nullptr);

        }

    }

    else if (command.find(std::string("to_free")) == 0){
        int inode_id = std::stoi(first_arg);
        file_bindings * current_file_bind = &files[inode_id];
        int fb_i = 0;

        do {
            files[inode_id].rw_mtx->lock();
            current_file_bind->n_of_further_parts_waiting_for_answer = files[inode_id].size_in_parts - fb_i;
            std::string command_to_slave = "command: to_free id: " + std::to_string(client_id) + " first_arg: " + std::to_string(current_file_bind->index_of_file) + " second_arg: " + std::to_string(current_file_bind->size_of_data) + " " + first_arg;
            participants_[current_file_bind->storage_slave_id]->write_possible_sequence(command_to_slave);

            current_file_bind = current_file_bind->next_part.get();
            fb_i++;
        }
        while(current_file_bind != nullptr);

    }
}

std::string Slaves_Group::get_fs_info(){
    counter_mutex->lock();
    std::string answer = "n_blocks: " + std::to_string(n_of_all_blocks) + ", free_blocks: " + std::to_string(all_free_blocks);
    counter_mutex->unlock();
    return answer;
}

int Slaves_Group::count_max_size_of_data_by_number_of_blocks(int n_blocks) {
    int added_block = 0;
    if (n_blocks % (NUMBER_OF_BYTES_IN_BLOCK / NUMBER_OF_CHARS_IN_INDEX) > 0)
        added_block = 1;

    // we subtract the number of address blocks that will be in the fs and that can't be used for storing chars
    return (n_blocks - added_block - n_blocks / (NUMBER_OF_BYTES_IN_BLOCK / NUMBER_OF_CHARS_IN_INDEX)) * NUMBER_OF_BYTES_IN_BLOCK;
}

int  Slaves_Group::get_size_of_data_in_fs_blocks(int size_of_data) {
    int added_block = 0;
    if (size_of_data * NUMBER_OF_CHARS_IN_INDEX % NUMBER_OF_BYTES_IN_BLOCK > 0)
        added_block = 1;
    int size = size_of_data * NUMBER_OF_CHARS_IN_INDEX / NUMBER_OF_BYTES_IN_BLOCK + added_block;

    added_block = 0;
    if (size_of_data % NUMBER_OF_BYTES_IN_BLOCK > 0)
        added_block = 1;

    size += size_of_data / NUMBER_OF_BYTES_IN_BLOCK + added_block;

    return size;
}

void Slaves_Group::send_data_to_slave(int inode_id, server_participant_ptr slave, std::string message, file_bindings &fb){
    int size_of_file_in_blocks = get_size_of_data_in_fs_blocks(fb.size_of_data);

    counter_mutex->lock();
    all_free_blocks -= size_of_file_in_blocks;
    counter_mutex->unlock();

    for (auto &it: slaves_info) {
        if (it.second.id == files[inode_id].storage_slave_id) {
            it.second.counter_mutex->lock();
            it.second.free_blocks -= size_of_file_in_blocks;
            it.second.counter_mutex->unlock();
            break;
        }
    }

    //storing partial information about data in slave, the data's location in slave will be filled out when
    //the slave sends it back
    fb.rw_mtx->lock();
    (*slave).write_possible_sequence(message);
}



void Slaves_Group::do_something_on_leave(int server_id) {}
