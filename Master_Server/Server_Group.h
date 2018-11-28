//
// Created by Сергей Ефимочкин on 22.11.2018.
//

#ifndef DFS_SERVER_GROUP_H
#define DFS_SERVER_GROUP_H

#include <vector>
#include "../utils/Server_Message.h"
#include "Inter_Server_Session.h"

class Server_Participant;
typedef std::shared_ptr<Server_Participant> server_participant_ptr;

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
};
#endif //DFS_SERVER_GROUP_H
