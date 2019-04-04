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
#include <sstream>
#include <fstream>


class Client_Server : public Server
{
public:
    Client_Server(boost::asio::io_service& io_service, tcp::resolver::iterator endpoint_iterator);



    ~Client_Server() = default;

/*
    void send_command(std::string &command, std::string &first_argument, std::string &second_argument){
        io_service_.post(
                [this, command, first_argument, second_argument]()
                {
                    std::cout << command;

                });
    }
*/
    void input_message(const std::string &command, std::string &first_arg, std::string &second_arg);

    void gain_control();


private:

    std::string parse_user_input_and_make_command(const std::string &message);

    void parse_answer_and_reply(const std::string &message) override;
    void parse_command_and_do_something(const std::string& message);


private:
    bool waiting_on_export = false;
    std::string name_of_export_file = "";
    std::mutex waiting_on_read;

};



#endif //DFS_CLIENT_SERVER_H
