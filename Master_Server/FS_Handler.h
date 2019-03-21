//
// Created by Сергей Ефимочкин on 2019-02-23.
//

#ifndef DFS_SLAVE_FS_HANDLER_H
#define DFS_SLAVE_FS_HANDLER_H

#include <string>
#include "../Base_Classes/Server_Group.h"
extern "C" {
    #include "../master_fs/FileSystem.h"
    #include "../master_fs/inode.h"
}
#include <unordered_map>
#include <mutex>
#include <boost/thread/shared_mutex.hpp>


class FS_Handler {
public:

    FS_Handler(): fs_mutex(new std::mutex()){

        filesystem = get_memory_for_filesystem();

        root = open_filesystem(name, (char*)filesystem, this);

        sb = (struct superblock *) filesystem;

        for (int i = 0; i < 1024; i++){
            inode *cur_inode = &(sb->inods_array[i]);
            inode_locks.emplace(cur_inode, new boost::shared_mutex());
        }
    }

    std::string do_command(int client_id, const std::string& command, const std::string& first_arg, const std::string& second_arg);

    void add_pointer_to_slaves_group(Server_Group *slaves_group){
        slaves_group_ = slaves_group;
    }

    void store_data_in_slave(int id,  char *name, char *data) {
        std::string
                message = "command: to_store id: " + std::to_string(id) + " first_arg: " + name + " second_arg: " + data;
        slaves_group_->send_command(message);
    }
    void read_data_in_slave(int id, char *name) {
        std::string
                message = "command: to_read id: " + std::to_string(id) + " first_arg: " + name + " second_arg: ";
        slaves_group_->send_command(message);
    }
    void free_data_in_slave(int id, char *name) {
        std::string
                message = "command: to_free id: " + std::to_string(id) + " first_arg: " + name + " second_arg: ";
        free(name);
        slaves_group_->send_command(message);
    }

    int get_size_of_data_in_FS_blocks(int size_of_data){
        return get_size_of_data_in_blocks(sb, size_of_data);
    }

    void client_leave(int client_id){
        struct inode** current_directory = clients_cur_directories[client_id];
        while (&(sb->inods_array[(*current_directory)->index_of_owner_inode]) != sb->inods_array){ //going up to the root
            inode_locks[*current_directory]->unlock_shared();
            *current_directory = &(sb->inods_array[(*current_directory)->index_of_owner_inode]);
        }
    }

    Server_Group * slaves_group_;
    std::shared_ptr<std::mutex> fs_mutex;
    std::unordered_map<struct inode*, std::shared_ptr<boost::shared_mutex>> inode_locks;

    struct inode_data{
        inode_data() :rw_mtx(new boost::shared_mutex()){}

        std::shared_ptr<boost::shared_mutex> rw_mtx;
    };

private:
    char * name = const_cast<char *>("./fs_names_and_directories");
    std::unordered_map<int,  struct inode**> clients_cur_directories;
    void* filesystem;
    struct inode* root;
    struct superblock* sb;

    int file_blocks_left = 1024;
    int number_of_blocks_left = 16384;

};





#endif //DFS_SLAVE_FS_HANDLER_H
