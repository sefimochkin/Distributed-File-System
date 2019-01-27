//
// Created by Сергей Ефимочкин on 22.11.2018.
//

#ifndef DFS_SERVER_GROUP_H
#define DFS_SERVER_GROUP_H

#include <vector>
#include "../utils/Server_Message.h"
#include "Inter_Server_Session.h"
#include <unordered_map>

class Server_Participant;
typedef std::shared_ptr<Server_Participant> server_participant_ptr;

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

class Server_Group
{
public:

  Server_Group();

  void join(server_participant_ptr server);

  void leave(server_participant_ptr server);

  void deliver(const Server_Message& msg);

  int len();

  void ping();

  void parse(server_participant_ptr slave, char* msg);


private:
    std::vector<server_participant_ptr> participants_;
    enum { max_recent_msgs = 100 };
    chat_message_queue recent_msgs_;
    int sum_of_overall_memory = 0;
    std::unordered_map<server_participant_ptr, slave_info> slaves_info;

};
#endif //DFS_SERVER_GROUP_H
