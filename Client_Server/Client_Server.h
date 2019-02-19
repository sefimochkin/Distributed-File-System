//
// Created by Сергей Ефимочкин on 2019-01-28.
//

#ifndef DFS_CLIENT_SERVER_H
#define DFS_CLIENT_SERVER_H

#include <cstdlib>
#include <deque>
#include <string>
#include "../Base_Classes/Server.h"
#include <iostream>
#include <cstdio>


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


    void send_command(std::string &command, std::string &first_argument, std::string &second_argument){
        io_service_.post(
                [this, command, first_argument, second_argument]()
                {
                    std::cout << command;

                });
    }



private:



    void parse_answer_and_reply(std::string message){

        printf("received something!: %s", message.c_str());
        if (message.find(std::string("ping")) == 0) {
            write_possible_sequence("send received ping back");
        }
        else if (message.find(std::string("id")) == 0){
            sscanf(message.c_str(), "id: %d", &id);
        }


    }

};



#endif //DFS_CLIENT_SERVER_H
