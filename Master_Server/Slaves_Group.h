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

struct slave_info{
    slave_info(server_participant_ptr owner_, int overall_memory_, int reserved_memory_, int id_) :
            owner(owner_), overall_memory(overall_memory_), reserved_memory(reserved_memory_), id(id_){}

    server_participant_ptr owner;

    server_participant_ptr server_to_backup = nullptr;
    server_participant_ptr backup_slave = nullptr;

    bool is_main = false;
    bool is_backup = false;

    int overall_memory;
    int reserved_memory;

    int id;
};

struct file_bindings{
    file_bindings(){};
    file_bindings(int storage_slave_id_, int size_of_data_) :
            storage_slave_id(storage_slave_id_), size_of_data(size_of_data_), rw_mtx(new boost::shared_mutex()){
    }
    int storage_slave_id;
    int index_of_file = -1;
    int size_of_data;
    std::shared_ptr<boost::shared_mutex> rw_mtx;
};


class Slaves_Group : public Server_Group
{
public:

  void parse_command_and_do_something(std::string message);

  void send_command(std::string message);

  void parse(server_participant_ptr slave, std::string message);

  ~Slaves_Group() = default;

private:
    std::unordered_map<std::string, file_bindings> files;
    int sum_of_overall_memory = 0;
    std::unordered_map<server_participant_ptr, slave_info> slaves_info;
};

#endif //DFS_SERVER_GROUP_H
