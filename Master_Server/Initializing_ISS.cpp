//
// Created by Сергей Ефимочкин on 2019-04-05.
//
#include "Initializing_ISS.h"

Initializing_ISS::Initializing_ISS(tcp::socket socket, Server_Group& slave_group, Server_Group& clients_group)
: Inter_Server_Session(std::move(socket), clients_group),
clients_group_(clients_group),
slaves_group_(slave_group){
}

void Initializing_ISS::start()
{
    do_read_header();
}


void Initializing_ISS::do_read_body(int header_code){
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
                            boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                            [this, self](boost::system::error_code ec, std::size_t /*length*/)
                            {
                                if (!ec)
                                {
                                    printf("%s\n", read_msg_.body());
                                    std::string answer = std::string(read_msg_.body());
                                    if (answer.find(std::string("clien")) == 0){
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
