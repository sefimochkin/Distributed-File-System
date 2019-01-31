//
// Created by Сергей Ефимочкин on 2019-01-28.
//

#ifndef DFS_CLIENT_SERVER_H
#define DFS_CLIENT_SERVER_H

#include <cstdlib>
#include <deque>
#include <string>
#include "../Base_Classes/Server.h"


class Client_Server : public Server
{
public:
    Client_Server(boost::asio::io_service& io_service,
                tcp::resolver::iterator endpoint_iterator) :
                Server(io_service, endpoint_iterator)
    {
        Server_Message msg = Server_Message();
        msg.make_message("client");
        write(msg);
    }



    ~Client_Server() = default;




private:

    void parse_answer_and_reply(){
        std::string answer = std::string(read_msg_.body());


        if (answer.find(std::string("ping")) == 0) {
            Server_Message msg = Server_Message();
            msg.make_message("received ping back");
            write(msg);
        }
        else if (answer.find(std::string("start")) == 0) {
            write(Server_Message("started"));
        }


    }

};



#endif //DFS_CLIENT_SERVER_H
