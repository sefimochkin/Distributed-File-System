//
// Created by Сергей Ефимочкин on 2019-02-19.
//

#ifndef DFS_SLAVE_CLIENTS_GROUP_H
#define DFS_SLAVE_CLIENTS_GROUP_H

#include "../Base_Classes/Server_Group.h"
#include "FS_Handler.h"

class Clients_Group : public Server_Group {

public:
    Clients_Group()=default;

    void send_command(std::string message);


    void parse(server_participant_ptr slave, std::string message);


    ~Clients_Group() = default;

    void add_pointer_to_other_group(Server_Group *other_group){
        other_group_ = other_group;
        fs.add_pointer_to_slaves_group(other_group);
    }

private:
    void parse_command_and_do_something(std::string message);
    FS_Handler fs;
};


#endif //DFS_SLAVE_CLIENTS_GROUP_H
