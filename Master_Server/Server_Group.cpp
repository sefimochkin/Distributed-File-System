//
// Created by Сергей Ефимочкин on 22.11.2018.
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

void Server_Group::parse(server_participant_ptr slave, char* msg){
    std::string answer = std::string(msg);

    if (answer.find(std::string("started")) == 0) {
        (*slave).deliver(Server_Message("overall size"));
    }


    else if (answer.find(std::string("overall size")) == 0){
        int overall_memory_of_slave;
        sscanf(answer.c_str(), "overall size: %d", &overall_memory_of_slave);
        sum_of_overall_memory += overall_memory_of_slave;
    }
}



Server_Group::Server_Group() {

}
