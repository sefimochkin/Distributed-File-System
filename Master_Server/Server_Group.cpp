//
// Created by Сергей Ефимочкин on 22.11.2018.
//

#include "Server_Group.h"

void Server_Group::join(server_participant_ptr server)
{
    participants_.insert(server);
    for (auto msg: recent_msgs_)
        server->deliver(msg);
}

void Server_Group::leave(server_participant_ptr server)
{
    participants_.erase(server);
}

void Server_Group::deliver(const Server_Message& msg)
{
    //recent_msgs_.push_back(msg);
    //while (recent_msgs_.size() > max_recent_msgs)
    //    recent_msgs_.pop_front();

    for (auto participant: participants_)
        participant->deliver(msg);
}

int Server_Group::len(){
    return participants_.size();
}

Server_Group::Server_Group() {

}
