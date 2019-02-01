//
// Created by Сергей Ефимочкин on 22.11.2018.
//

#ifndef DFS_SERVER_GROUP_H
#define DFS_SERVER_GROUP_H

#include <vector>
#include "../utils/Server_Message.h"
#include <unordered_map>
#include "../Base_Classes/Server_Group.h"

struct slave_info{
    slave_info(server_participant_ptr owner_, int overall_memory_, int reserved_memory_) :
            owner(owner_), overall_memory(overall_memory_), reserved_memory(reserved_memory_){}

    server_participant_ptr owner;

    server_participant_ptr server_to_backup = nullptr;
    server_participant_ptr backup_slave = nullptr;

    bool is_main = false;
    bool is_backup = false;

    int overall_memory;
    int reserved_memory;
};


class Slaves_Group : public Server_Group
{
public:

  void parse(server_participant_ptr slave, std::string message);

  ~Slaves_Group() = default;


private:
    int sum_of_overall_memory = 0;
    std::unordered_map<server_participant_ptr, slave_info> slaves_info;

};

#endif //DFS_SERVER_GROUP_H
