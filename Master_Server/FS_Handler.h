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

    FS_Handler(): fs_mutex(new std::mutex()), counter_mutex(new std::mutex()){

        filesystem = get_memory_for_filesystem(number_of_inodes, number_of_name_blocks);

        root = open_filesystem(name, (char*)filesystem, this, number_of_inodes, number_of_name_blocks);

        sb = (struct superblock *) filesystem;

        for (int i = 0; i < 1024; i++){
            inode *cur_inode = &(sb->inods_array[i]);
            inode_locks.emplace(cur_inode, new boost::shared_mutex());
        }
    }

    std::string do_command(int client_id, const std::string& command, const std::string& first_arg, const std::string& second_arg, short &error);

    void add_pointer_to_slaves_group(Server_Group *slaves_group){
        slaves_group_ = slaves_group;
    }

    void store_data_in_slave(int id, int inode_id, char *data) {
        std::string
                message = "command: to_store id: " + std::to_string(id) + " first_arg: " + std::to_string(inode_id) + " second_arg: " + data;
        slaves_group_->send_command(message);
    }
    void read_data_in_slave(int id, int inode_id) {
        std::string
                message = "command: to_read id: " + std::to_string(id) + " first_arg: " + std::to_string(inode_id) + " second_arg: ";
        slaves_group_->send_command(message);
    }
    void free_data_in_slave(int id, int inode_id) {
        std::string
                message = "command: to_free id: " + std::to_string(id) + " first_arg: " + std::to_string(inode_id) + " second_arg: ";
        slaves_group_->send_command(message);
    }

    int get_size_of_data_in_FS_blocks(int size_of_data){
        return get_size_of_data_in_blocks(sb, size_of_data);
    }

    std::string get_approximate_size_of_fs_blocks(int n_blocks){
        int size = sizeof(char) * get_approximate_size_of_blocks(sb, n_blocks);

        std::string answer;
        if (size > 1024)
            answer = std::to_string(size / 1024) + "kb" + std::to_string(size % 1024) + "b";
        else
            answer = std::to_string(size % 1024) + "b";

        return answer;
    }

    std::string get_fs_info(){
        int n_blocks, free_blocks;
        std::string fs_info = slaves_group_->get_fs_info();
        sscanf(fs_info.c_str(), "n_blocks: %d, free_blocks: %d", &n_blocks, &free_blocks);
        std::string answer = "files left: " + std::to_string(number_of_free_inodes) + "/" + std::to_string(number_of_inodes) + ", data_memory left: " + get_approximate_size_of_fs_blocks(free_blocks) + "/" + get_approximate_size_of_fs_blocks(n_blocks) + ", file_name_memory left: " + get_approximate_size_of_fs_blocks(number_of_free_name_blocks) + "/" + get_approximate_size_of_fs_blocks(number_of_name_blocks);
        return answer;
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

    std::string check_file(bool &error){
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

    std::string check_name(int size_of_data, bool &error){
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

    std::string check_data(int size_of_data, bool &error){
        std::string answer = "";
        int n_blocks, free_blocks;
        std::string fs_info = slaves_group_->get_fs_info();
        sscanf(fs_info.c_str(), "n_blocks: %d, free_blocks: %d", &n_blocks, &free_blocks);
        int tmp = get_size_of_data_in_FS_blocks(size_of_data);
        counter_mutex->lock();
        if (get_size_of_data_in_FS_blocks(size_of_data) > free_blocks){
            counter_mutex->unlock();
            answer = "There are no data blocks left! Delete something to create something new!";
            error = true;
        } else
            counter_mutex->unlock();
        return answer;
    }

    std::string check_directory(int size_of_name, bool &error){
        std::string answer = check_file(error);
        if (!error)
            answer = check_name(size_of_name, error);
        return answer;
    }

    std::string check_file(int size_of_name, int size_of_data, bool &error){
        std::string answer = check_file(error);
        if (!error) {
            answer = check_name(size_of_name, error);
            if (!error)
                answer = check_data(size_of_data, error);
        }
        return answer;
    }


    void add_file(int size_of_name){
        counter_mutex->lock();
        number_of_free_inodes--;
        number_of_free_name_blocks -= get_size_of_data_in_FS_blocks(size_of_name);
        counter_mutex->unlock();
    }

    void rm_file(int size_of_name){
        counter_mutex->lock();
        number_of_free_inodes++;
        number_of_free_name_blocks += get_size_of_data_in_FS_blocks(size_of_name);
        counter_mutex->unlock();
    }

    char * name = const_cast<char *>("./fs_names_and_directories");
    std::unordered_map<int,  struct inode**> clients_cur_directories;
    void* filesystem;
    struct inode* root;
    struct superblock* sb;


    std::shared_ptr<std::mutex> counter_mutex;
    int number_of_name_blocks = 16384;
    int number_of_free_name_blocks = 16382;

    int number_of_inodes = 1024;
    int number_of_free_inodes = 1024;

};





#endif //DFS_SLAVE_FS_HANDLER_H
