//
// Created by Сергей Ефимочкин on 2019-01-30.
//

#ifndef DFS_INITIALIZING_ISS_H
#define DFS_INITIALIZING_ISS_H

#include "Initializing_ISS.h"
#include "../Base_Classes/Inter_Server_Session.h"

class Initializing_ISS: public Inter_Server_Session{

public:
    Initializing_ISS(tcp::socket socket, Server_Group& slave_group, Server_Group& clients_group)
    : Inter_Server_Session(std::move(socket), clients_group),
      clients_group_(clients_group),
      slaves_group_(slave_group){
    }

    void start()
    {
        do_read_header();
    }


private:
    void do_read_body(int header_code){
        auto self(shared_from_this());
        boost::asio::async_read(socket_,
                                boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                                [this, self](boost::system::error_code ec, std::size_t /*length*/)
                                {
                                    if (!ec)
                                    {
                                        printf("%s\n", read_msg_.body());
                                        std::string answer = std::string(read_msg_.body());
                                        if (answer.find(std::string("client")) == 0){
                                            std::make_shared<Inter_Server_Session>(std::move(socket_), clients_group_)->start();
                                            printf("client connected!\n");
                                        }
                                        else if (answer.find(std::string("slave")) == 0){
                                            std::make_shared<Inter_Server_Session>(std::move(socket_), slaves_group_)->start();
                                            printf("slave connected!\n");
                                        }

                                    }
                                });
    }

private:
    Server_Group& slaves_group_;
    Server_Group& clients_group_;

};

#endif //DFS_INITIALIZING_ISS_H
