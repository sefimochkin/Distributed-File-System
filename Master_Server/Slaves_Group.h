//
// Created by Сергей Ефимочкин on 22.11.2018.
//

#ifndef DFS_SERVER_GROUP_H
#define DFS_SERVER_GROUP_H

#include <vector>
#include "../utils/Server_Message.h"
#include <unordered_map>
#include "../Base_Classes/Server_Group.h"

class Slaves_Group : public Server_Group
{
public:

  void parse(server_participant_ptr slave, char* msg);

  ~Slaves_Group() = default;


private:
    std::vector<server_participant_ptr> participants_;
    enum { max_recent_msgs = 100 };
    chat_message_queue recent_msgs_;
    int sum_of_overall_memory = 0;
    std::unordered_map<server_participant_ptr, slave_info> slaves_info;

};

#endif //DFS_SERVER_GROUP_H
