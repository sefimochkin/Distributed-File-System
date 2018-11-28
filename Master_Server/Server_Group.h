//
// Created by Сергей Ефимочкин on 22.11.2018.
//

#ifndef DFS_SERVER_GROUP_H
#define DFS_SERVER_GROUP_H

#include <set>
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


  std::set<server_participant_ptr> participants_;
private:
  enum { max_recent_msgs = 100 };
  chat_message_queue recent_msgs_;
};
#endif //DFS_SERVER_GROUP_H
