//
// Created by Сергей Ефимочкин on 22.11.2018.
//

#ifndef DFS_SERVER_GROUP_H
#define DFS_SERVER_GROUP_H

#include <vector>
#include "../utils/Server_Message.h"
#include <unordered_map>
#include "../Base_Classes/Server_Group.h"
#include "Clients_Group.h"
#include <boost/thread/shared_mutex.hpp>

#define NUMBER_OF_BYTES_IN_BLOCK  32   //sorry for this, but I don't have enough time to clean this up, so it's up to you
#define NUMBER_OF_CHARS_IN_INDEX  4

struct slave_info{
    slave_info(server_participant_ptr owner_, int n_blocks_, int free_blocks_, int id_) :
            owner(owner_), n_blocks(n_blocks_), free_blocks(free_blocks_), id(id_), counter_mutex(new boost::mutex()){}

    server_participant_ptr owner;

    server_participant_ptr server_to_backup = nullptr;
    server_participant_ptr backup_slave = nullptr;

    bool is_main = false;
    bool is_backup = false;

    int n_blocks;
    int free_blocks;

    int id;

    std::shared_ptr<boost::mutex> counter_mutex;

    bool operator < (const slave_info& other_slave_info) const
    {
        counter_mutex->lock();
        other_slave_info.counter_mutex->lock();
        bool ans = free_blocks < other_slave_info.free_blocks;
        counter_mutex->unlock();
        other_slave_info.counter_mutex->unlock();
        return ans;
    }
};

struct file_bindings{
    file_bindings(){};
    file_bindings(int storage_slave_id_, int size_of_data_) :
            storage_slave_id(storage_slave_id_), size_of_data(size_of_data_), rw_mtx(new boost::shared_mutex()){
    }
    int storage_slave_id;
    int index_of_file = -1;
    int size_of_data;
    int size_of_file_in_blocks;
    std::shared_ptr<boost::shared_mutex> rw_mtx;
};

struct inverse_slave_info_map_comparator {
    bool operator()(std::pair<const server_participant_ptr, slave_info> const&left,
                    std::pair<const server_participant_ptr, slave_info> const&right) const {
        return  right.second < left.second; // swap it so we would have reverse order sorted array
    }
};


class Slaves_Group : public Server_Group
{
public:

  Slaves_Group() : counter_mutex(new boost::mutex()){}

  void parse_command_and_do_something(std::string message);

  void send_command(std::string message);

  void parse(server_participant_ptr slave, std::string message);

  void do_something_on_leave(int server_id);

  std::string get_fs_info();

  ~Slaves_Group() = default;

private:
    int count_max_size_of_data_by_number_of_blocks(int size_of_data);

    int  get_size_of_data_in_fs_blocks(int size_of_data);

    void send_data_to_slave(int inode_id, server_participant_ptr slave, std::string message, file_bindings &fb);

private:
    std::unordered_map<int, file_bindings> files;
    int n_of_all_blocks = 0;
    int all_free_blocks = 0;
    std::unordered_map<server_participant_ptr, slave_info> slaves_info;

    std::shared_ptr<boost::mutex> counter_mutex;
};

#endif //DFS_SERVER_GROUP_H
