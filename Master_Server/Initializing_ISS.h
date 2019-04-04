//
// Created by Сергей Ефимочкин on 2019-01-30.
//

#ifndef DFS_INITIALIZING_ISS_H
#define DFS_INITIALIZING_ISS_H

#include "Initializing_ISS.h"
#include "../Base_Classes/Inter_Server_Session.h"

class Initializing_ISS: public Inter_Server_Session{

public:
    Initializing_ISS(tcp::socket socket, Server_Group& slave_group, Server_Group& clients_group);

    void start();


private:
    void do_read_body(int header_code);

private:
    Server_Group& slaves_group_;
    Server_Group& clients_group_;

};

#endif //DFS_INITIALIZING_ISS_H
