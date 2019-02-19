//
// Created by Сергей Ефимочкин on 2019-02-19.
//

#ifndef DFS_SLAVE_CLIENTS_GROUP_H
#define DFS_SLAVE_CLIENTS_GROUP_H

#include "../Base_Classes/Server_Group.h"
#include "Slaves_Group.h"

class Clients_Group : public Server_Group {

public:
    Clients_Group() = default;
    Clients_Group(Slaves_Group *slaves_group_){
        slaves_group = slaves_group_;
    }
    void parse(server_participant_ptr slave, std::string message);

    ~Clients_Group() = default;

private:
    Slaves_Group* slaves_group;
};


#endif //DFS_SLAVE_CLIENTS_GROUP_H
