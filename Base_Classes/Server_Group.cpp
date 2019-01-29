//
// Created by Сергей Ефимочкин on 2019-01-30.
//


#include "Server_Group.h"

void Server_Group::join(server_participant_ptr server)
{
    if (std::find(participants_.begin(), participants_.end(), server)==participants_.end()) {
        participants_.push_back(server);
        (*server).deliver(Server_Message("start"));
    }

    for (auto msg: recent_msgs_)
        server->deliver(msg);
}

void Server_Group::leave(server_participant_ptr server)
{
    participants_.erase(std::remove(participants_.begin(), participants_.end(), server), participants_.end());
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

void Server_Group::ping(){
    Server_Message msg = Server_Message();
    msg.make_message("ping");
    deliver(msg);
}


Server_Group::Server_Group() {
}
