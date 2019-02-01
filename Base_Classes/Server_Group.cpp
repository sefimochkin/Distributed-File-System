//
// Created by Сергей Ефимочкин on 2019-01-30.
//


#include "Server_Group.h"

void Server_Group::join(server_participant_ptr server)
{
    if (std::find(participants_.begin(), participants_.end(), server)==participants_.end()) {
        participants_.push_back(server);
        std::string id_string = "id: " + std::to_string(current_id);
        current_id++;
        continuous_messages.insert({server, std::string("")});
        (*server).write_possible_sequence(id_string);

    }

    //for (auto msg: recent_msgs_)
    //    server->write_possible_sequence(msg);
}

void Server_Group::leave(server_participant_ptr server)
{
    participants_.erase(std::remove(participants_.begin(), participants_.end(), server), participants_.end());
}

void Server_Group::deliver(const std::string message)
{
    //recent_msgs_.push_back(msg);
    //while (recent_msgs_.size() > max_recent_msgs)
    //    recent_msgs_.pop_front();

    for (auto participant: participants_)
        participant->write_possible_sequence(message);
}

int Server_Group::len(){
    return participants_.size();
}

void Server_Group::ping(){
    deliver("ping");
}

void Server_Group::parse_possible_sequence(server_participant_ptr server, std::string message, int header_code){
    if (header_code == 2) {
        if (strcmp(continuous_messages[server].c_str(), "") == 0) {
            std::cout << continuous_messages[server] << "\n";
            parse(server, message);
        } else {
            continuous_messages[server] = continuous_messages[server] + message;
            std::cout << continuous_messages[server] << "\n";
            parse(server, continuous_messages[server]);
            continuous_messages[server] = "";
        }
    }
    else{
        continuous_messages[server] = continuous_messages[server] + message;
    }

}



Server_Group::Server_Group() {
}
