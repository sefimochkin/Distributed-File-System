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

    FS_Handler();

    std::string do_command(int client_id, const std::string& command, const std::string& first_arg, const std::string& second_arg);

    void add_pointer_to_slaves_group(Server_Group *slaves_group);

    void store_data_in_slave(int id,  int inode_id, char *data);
    void read_data_in_slave(int id, int inode_id);
    void free_data_in_slave(int id, int inode_id);

    int get_size_of_data_in_FS_blocks(int size_of_data);
    std::string get_approximate_size_of_fs_blocks(int n_blocks);
    std::string get_fs_info();

    void client_leave(int client_id);

    Server_Group * slaves_group_;
    std::shared_ptr<std::mutex> fs_mutex;
    std::unordered_map<struct inode*, std::shared_ptr<boost::shared_mutex>> inode_locks;

    struct inode_data{
        inode_data() :rw_mtx(new boost::shared_mutex()){}

        std::shared_ptr<boost::shared_mutex> rw_mtx;
    };

private:

    std::string check_file(bool &error);
    std::string check_name(int size_of_data, bool &error);
    std::string check_data(int size_of_data, bool &error);
    std::string check_directory(int size_of_name, bool &error);
    std::string check_new_file(int size_of_name, int size_of_data, bool &error);


    void add_file(int size_of_name);
    void rm_file(int size_of_name);

    char * name = const_cast<char *>("./fs_names_and_directories");
    std::unordered_map<int,  struct inode**> clients_cur_directories;
    void* filesystem;
    struct inode* root;
    struct superblock* sb;


    std::shared_ptr<std::mutex> counter_mutex;
    int number_of_name_blocks = 16384;
    int number_of_free_name_blocks = 16384;

    int number_of_inodes = 1024;
    int number_of_free_inodes = 1024;

};





#endif //DFS_SLAVE_FS_HANDLER_H
