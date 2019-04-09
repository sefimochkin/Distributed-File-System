//
// Created by Сергей Ефимочкин on 2019-02-23.
//

#include "FS_Handler.h"
//#include "../master_fs/FileSystem.h"
#include <algorithm>
#include <memory>

FS_Handler::FS_Handler(): fs_mutex(new std::mutex()), counter_mutex(new std::mutex()){

    filesystem = get_memory_for_filesystem(number_of_inodes, number_of_name_blocks);

    root = open_filesystem(name, (char*)filesystem, this, number_of_inodes, number_of_name_blocks);

    sb = (struct superblock *) filesystem;

    for (int i = 0; i < 1024; i++){
        inode *cur_inode = &(sb->inods_array[i]);
        inode_locks.emplace(cur_inode, new boost::shared_mutex());
    }
}

std::string FS_Handler::do_command(int client_id, const std::string& command, const std::string& first_arg, const std::string& second_arg, short &failed){
    printf("FS_Handler::do_command\n");

    printf("%d\n", client_id);
    auto it = clients_cur_directories.find(client_id);

    if(it == clients_cur_directories.end())
    {
        inode * root_copy = root;
        clients_cur_directories.insert({client_id, std::make_shared<struct inode *>(root_copy)});
    }

    int number_of_arguments = 0;
    if (command.length() > 0)
        number_of_arguments++;
    if (first_arg.length() > 0)
        number_of_arguments++;
    if (second_arg.length() > 0)
        number_of_arguments++;

    std::string answer;

    if(command.find(std::string("ls")) == 0){
        char* output = ls(sb, clients_cur_directories[client_id].get(), this);
        answer = std::string(output);
        std::replace( answer.begin(), answer.end(), '\n', ' ');
        if(is_directory_with_files(clients_cur_directories[client_id].get()))
            free(output);
    }

    else if (command.find(std::string("mkdir")) == 0){
        if(number_of_arguments == 1)
            answer =  std::string("Not enough arguments!");
        else {
            bool error = false;
            answer = check_directory(first_arg.length(), error);
            if (!error) {
                add_file(first_arg.length());
                answer = mkdirf(sb, first_arg.c_str(), clients_cur_directories[client_id].get(), this);
            }
        }
    }

    else if (command.find(std::string("rmdir")) == 0){
        if(number_of_arguments == 1)
            answer =  std::string("Not enough arguments!");
        else {
            rm_file(first_arg.length());
            answer = rm_dir(sb, first_arg.c_str(), clients_cur_directories[client_id].get(), this, client_id);
        }
    }

    else if (command.find(std::string("touch")) == 0){
        if(number_of_arguments < 3)
            answer =  std::string("Not enough arguments!");
        else {
            bool error = false;
            answer = check_new_file(first_arg.length(), second_arg.length(), error);
            if (!error) {
                add_file(first_arg.length());

                answer = touch(sb, first_arg.c_str(), second_arg.c_str(), clients_cur_directories[client_id].get(), this,
                               client_id);
            }
        }
    }

    else if (command.find(std::string("rm")) == 0){
        if(number_of_arguments == 1)
            answer =  std::string("Not enough arguments!");
        else {
            rm_file(first_arg.length());
            answer = rm(sb, first_arg.c_str(), clients_cur_directories[client_id].get(), this, client_id);
        }
    }

    else if (command.find(std::string("read")) == 0){
        if(number_of_arguments == 1)
            answer =  std::string("Not enough arguments!");
        else {
            printf("client id: %d\n", client_id);
            char *output = read_file(sb, first_arg.c_str(), clients_cur_directories[client_id].get(), &failed, this, client_id);
            //if (!failed)
            printf("failed: %d\n", failed);
            answer =  std::string(output);
            //if (failed == 2) {
            //    inode * root_copy = root;
            //    clients_cur_directories[client_id] = &root_copy;
            //}

        }
    }

    else if (command.find(std::string("cd")) == 0){
        if(number_of_arguments == 1)
            answer =  std::string("Not enough arguments!");
        else {
            answer = cd(sb, first_arg.c_str(), clients_cur_directories[client_id].get(), this);
        }
    }

    else if (command.find(std::string("import")) == 0){
        if(number_of_arguments < 3)
            answer =  std::string("Not enough arguments!");
        else {
            bool error = false;
            answer = check_new_file(first_arg.length(), second_arg.length(), error);
            if (!error) {
                add_file(first_arg.length());
                answer = touch(sb, first_arg.c_str(), second_arg.c_str(), clients_cur_directories[client_id].get(), this,
                               client_id);
            }
        }
    }

    else if (command.find(std::string("export")) == 0){
        if(number_of_arguments < 3)
            answer =  std::string("Not enough arguments!");
        else {
            short failed = 0;
            char *output = read_file(sb, first_arg.c_str(), clients_cur_directories[client_id].get(), &failed, this, client_id);
            answer =  std::string(output);
            //if(failed != 1) {
            //    free(output);
            //}

        }
    }

    else if (command.find(std::string("save")) == 0){
        answer = save_filesystem(name, (char *) filesystem);
    }

    else if (command.find(std::string("fs_info")) == 0){
        answer = get_fs_info();
    }

    else if (command.find(std::string("help")) == 0){
        answer =  std::string("ls to list files in current directory\ncd $name$ to go to directory named $name$\n"
                          "mkdir $name$ to create directory named $name$\ntouch $name$ $file text$ to create a file "
                          "named $name$ with text of file $file text$\nrmdir $name$ to delete directory named $name$ "
                          "and all files in it\nrm $name$ to delete file named $name$ from directory\n"
                          "read $name$ to print text of file named $name$\nimport $outer name$ $inner name$ to import "
                          "file named $outer name$ from computer's file system and save it as file named $inner name$ in "
                          "this file system\nexport $inner name$ $outer name$ to export file named $inner name$ into "
                          "computer's file system as a file named $outername$\nexit to exit");
    }

    else if (command.find(std::string("exit")) == 0) {
        answer =  std::string("");
    }
/*
    else if (command.find(std::string("stop")) == 0){
        running = 0;
        answer =  std::string("");
    }
*/
    else { //in case of unknown command or a misspell
        answer =  std::string("");
    }
    return answer;

}

void FS_Handler::add_pointer_to_slaves_group(Server_Group *slaves_group){
    slaves_group_ = slaves_group;
}

void FS_Handler::store_data_in_slave(int id,  int inode_id, char *data) {
    printf("FS_Handler::store_data_in_slave\n");
    std::string
            message = "command: to_store id: " + std::to_string(id) + " first_arg: " + std::to_string(inode_id) + " second_arg: " + data;
    slaves_group_->send_command(message);
}

void FS_Handler::read_data_in_slave(int id, int inode_id) {
    printf("FS_Handler::read_data_in_slave\n");
    std::string
            message = "command: to_read id: " + std::to_string(id) + " first_arg: " + std::to_string(inode_id) + " second_arg: ";
    slaves_group_->send_command(message);
}

void FS_Handler::free_data_in_slave(int id, int inode_id) {
    printf("FS_Handler::free_data_in_slave\n");
    std::string
            message = "command: to_free id: " + std::to_string(id) + " first_arg: " + std::to_string(inode_id) + " second_arg: ";
    slaves_group_->send_command(message);
}

int FS_Handler::get_size_of_data_in_FS_blocks(int size_of_data){
    return get_size_of_data_in_blocks(sb, size_of_data);
}

std::string FS_Handler::get_approximate_size_of_fs_blocks(int n_blocks){
    int size = sizeof(char) * get_approximate_size_of_blocks(sb, n_blocks);

    std::string answer;
    if (size > 1024)
        answer = std::to_string(size / 1024) + "kb" + std::to_string(size % 1024) + "b";
    else
        answer = std::to_string(size % 1024) + "b";

    return answer;
}

std::string FS_Handler::get_fs_info(){
    int n_blocks, free_blocks;
    std::string fs_info = slaves_group_->get_fs_info();
    sscanf(fs_info.c_str(), "n_blocks: %d, free_blocks: %d", &n_blocks, &free_blocks);
    std::string answer = "files left: " + std::to_string(number_of_free_inodes) + "/" + std::to_string(number_of_inodes) + ", data_memory left: " + get_approximate_size_of_fs_blocks(free_blocks) + "/" + get_approximate_size_of_fs_blocks(n_blocks) + ", file_name_memory left: " + get_approximate_size_of_fs_blocks(number_of_free_name_blocks) + "/" + get_approximate_size_of_fs_blocks(number_of_name_blocks);
    return answer;
}

void FS_Handler::client_leave(int client_id){
    struct inode** current_directory = clients_cur_directories[client_id].get();
    while (&(sb->inods_array[(*current_directory)->index_of_owner_inode]) != sb->inods_array){ //going up to the root
        inode_locks[*current_directory]->unlock_shared();
        *current_directory = &(sb->inods_array[(*current_directory)->index_of_owner_inode]);
    }
}

std::string FS_Handler::check_file(bool &error){
    std::string answer = "";
    counter_mutex->lock();
    if (number_of_free_inodes == 0){
        counter_mutex->unlock();
        answer = "There are no file nodes left! Delete something to create something new!";
        error = true;
    } else
        counter_mutex->unlock();
    return answer;
}

std::string FS_Handler::check_name(int size_of_data, bool &error){
    std::string answer = "";
    counter_mutex->lock();
    if (get_size_of_data_in_FS_blocks(size_of_data) > number_of_free_name_blocks){
        counter_mutex->unlock();
        answer = "There are no name blocks left! Delete something to create something new!";
        error = true;
    } else
        counter_mutex->unlock();
    return answer;
}

std::string FS_Handler::check_data(int size_of_data, bool &error){
    std::string answer = "";
    int n_blocks, free_blocks;
    std::string fs_info = slaves_group_->get_fs_info();
    sscanf(fs_info.c_str(), "n_blocks: %d, free_blocks: %d", &n_blocks, &free_blocks);
    counter_mutex->lock();
    if (get_size_of_data_in_FS_blocks(size_of_data) > free_blocks){
        counter_mutex->unlock();
        answer = "There are no data blocks left! Delete something to create something new!";
        error = true;
    } else
        counter_mutex->unlock();
    return answer;
}

std::string FS_Handler::check_directory(int size_of_name, bool &error){
    std::string answer = check_file(error);
    if (!error)
        answer = check_name(size_of_name, error);
    return answer;
}

std::string FS_Handler::check_new_file(int size_of_name, int size_of_data, bool &error){
    std::string answer = check_file(error);
    if (!error) {
        answer = check_name(size_of_name, error);
        if (!error)
            answer = check_data(size_of_data, error);
    }
    return answer;
}


void FS_Handler::add_file(int size_of_name){
    counter_mutex->lock();
    number_of_free_inodes--;
    number_of_free_name_blocks -= get_size_of_data_in_FS_blocks(size_of_name);
    counter_mutex->unlock();
}

void FS_Handler::rm_file(int size_of_name){
    counter_mutex->lock();
    number_of_free_inodes++;
    number_of_free_name_blocks += get_size_of_data_in_FS_blocks(size_of_name);
    counter_mutex->unlock();
}



extern "C" void store_data_in_slave_wrapper(FS_Handler * fs_handler, int id,  int inode_id, char *data) {
    fs_handler->store_data_in_slave(id, inode_id, data);
}

extern "C" void read_data_in_slave_wrapper(FS_Handler * fs_handler, int id, int inode_id) {
    fs_handler->read_data_in_slave(id, inode_id);
}
extern "C" void free_data_in_slave_wrapper(FS_Handler * fs_handler, int id, int inode_id) {
    fs_handler->free_data_in_slave(id, inode_id);
}

extern "C" void lock_fs_mutex(FS_Handler * fs_handler) {
    fs_handler->fs_mutex->lock();
}

extern "C" void unlock_fs_mutex(FS_Handler * fs_handler) {
    fs_handler->fs_mutex->unlock();
}

extern "C" void lock_inode_mutex(FS_Handler * fs_handler, struct inode* inode) {
    fs_handler->inode_locks[inode]->lock();
}

extern "C" void unlock_inode_mutex(FS_Handler * fs_handler, struct inode* inode) {
    fs_handler->inode_locks[inode]->unlock();
}

extern "C" void shared_lock_inode_mutex(FS_Handler * fs_handler, struct inode* inode) {
    fs_handler->inode_locks[inode]->lock_shared();
}

extern "C" void shared_unlock_inode_mutex(FS_Handler * fs_handler, struct inode* inode) {
    fs_handler->inode_locks[inode]->unlock_shared();
}

extern "C" short try_lock_inode_mutex(FS_Handler * fs_handler, struct inode* inode) {
    if(fs_handler->inode_locks[inode]->try_lock())
        return 1;
    else
        return 0;
}


