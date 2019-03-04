//
// Created by Сергей Ефимочкин on 2019-02-23.
//

#ifndef DFS_SLAVE_FS_HANDLER_H
#define DFS_SLAVE_FS_HANDLER_H

#include <string>
#include "../Base_Classes/Server_Group.h"
extern "C" {
    #include "../fs/FileSystem.h"
//#include "../fs/inode.h"
}
#include <unordered_map>

class FS_Handler {
public:

    FS_Handler(){

        filesystem = get_memory_for_filesystem();

        root = open_filesystem(name, (char*)filesystem);

        sb = (struct superblock *) filesystem;
    }

    std::string do_command(int client_id, const std::string& command, const std::string& first_arg, const std::string& second_arg);

    void add_pointer_to_slaves_group(Server_Group *slaves_group){
        slaves_group_ = slaves_group;
    }

    Server_Group * slaves_group_;

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

private:
    char * name = const_cast<char *>("./fs_names_and_directories");
    std::unordered_map<int,  struct inode**> clients_cur_directories;
    void* filesystem;
    struct inode* root;
    struct superblock* sb;

};





#endif //DFS_SLAVE_FS_HANDLER_H
