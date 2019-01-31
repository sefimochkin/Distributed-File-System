//
// Created by Сергей Ефимочкин on 2019-01-30.
//

#ifndef DFS_BASE_SERVER_GROUP_H
#define DFS_BASE_SERVER_GROUP_H

#include <vector>
#include "../utils/Server_Message.h"
#include "../Master_Server/Inter_Server_Session.h"
#include <unordered_map>

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

    virtual void parse(server_participant_ptr slave, char* msg) = 0;

    virtual ~Server_Group() = default;

protected:
    std::vector<server_participant_ptr> participants_;
    enum { max_recent_msgs = 100 };
    chat_message_queue recent_msgs_;

};
#endif //DFS_BASE_SERVER_GROUP_H
