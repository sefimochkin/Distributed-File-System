//
// Created by Сергей Ефимочкин on 2019-02-23.
//

#include "FS_Handler.h"
//#include "../master_fs/FileSystem.h"
#include <algorithm>


std::string FS_Handler::do_command(int client_id, const std::string& command, const std::string& first_arg, const std::string& second_arg, short &error){
    printf("%d\n", client_id);
    auto it = clients_cur_directories.find(client_id);

    if(it == clients_cur_directories.end())
    {
        inode * root_copy = root;
        clients_cur_directories[client_id] = &root_copy;
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
        char* output = ls(sb, *clients_cur_directories[client_id], this);
        answer = std::string(output);
        std::replace( answer.begin(), answer.end(), '\n', ' ');
        if((*clients_cur_directories[client_id])->number_of_files_in_directory > 0)
            free(output);
    }

    else if (command.find(std::string("mkdir")) == 0){
        if(number_of_arguments == 1)
            answer =  std::string("Not sufficient arguments!");
        else {
            bool error = false;
            answer = check_directory(first_arg.length(), error);
            if (!error) {
                add_file(first_arg.length());
                answer = mkdirf(sb, first_arg.c_str(), *clients_cur_directories[client_id], this);
            }
        }
    }

    else if (command.find(std::string("rmdir")) == 0){
        if(number_of_arguments == 1)
            answer =  std::string("Not sufficient arguments!");
        else {
            rm_file(first_arg.length());
            answer = rm_dir(sb, first_arg.c_str(), *clients_cur_directories[client_id], this, client_id);
        }
    }

    else if (command.find(std::string("touch")) == 0){
        if(number_of_arguments < 3)
            answer =  std::string("Not sufficient arguments!");
        else {
            bool error = false;
            answer = check_file(first_arg.length(), second_arg.length(), error);
            if (!error) {
                add_file(first_arg.length());
                answer = touch(sb, first_arg.c_str(), second_arg.c_str(), *clients_cur_directories[client_id], this,
                               client_id);
            }
        }
    }

    else if (command.find(std::string("rm")) == 0){
        if(number_of_arguments == 1)
            answer =  std::string("Not sufficient arguments!");
        else {
            rm_file(first_arg.length());
            answer = rm(sb, first_arg.c_str(), *clients_cur_directories[client_id], this, client_id);
        }
    }

    else if (command.find(std::string("read")) == 0){
        if(number_of_arguments == 1)
            answer =  std::string("Not sufficient arguments!");
        else {
            char *output = read_file(sb, first_arg.c_str(), *clients_cur_directories[client_id], &error, this, client_id);
            if (error == 0)
                answer =  std::string(output);
            else
                answer = std::string("Can't find a file with this name!");
        }
    }

    else if (command.find(std::string("cd")) == 0){
        if(number_of_arguments == 1)
            answer =  std::string("Not sufficient arguments!");
        else {
            answer = cd(sb, first_arg.c_str(), clients_cur_directories[client_id], this);
        }
    }

    else if (command.find(std::string("import")) == 0){
        if(number_of_arguments < 3)
            answer =  std::string("Not sufficient arguments!");
        else {
            bool error = false;
            answer = check_file(first_arg.length(), second_arg.length(), error);
            if (!error) {
                add_file(first_arg.length());
                answer = touch(sb, first_arg.c_str(), second_arg.c_str(), *clients_cur_directories[client_id], this,
                               client_id);
            }
        }
    }

    else if (command.find(std::string("export")) == 0){
        if(number_of_arguments < 3)
            answer =  std::string("Not sufficient arguments!");
        else {
            short failed = 0;
            char *output = read_file(sb, first_arg.c_str(), *clients_cur_directories[client_id], &failed, this, client_id);
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
                          "computer's file system as a file named $outername$\nsave to save all changes made in "
                          "the filesystem\nexit to save and exit");
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


