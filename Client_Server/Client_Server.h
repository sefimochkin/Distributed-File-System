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

/*
    void send_command(std::string &command, std::string &first_argument, std::string &second_argument){
        io_service_.post(
                [this, command, first_argument, second_argument]()
                {
                    std::cout << command;

                });
    }
*/
    void input_message(const std::string &command, const std::string &first_arg, const std::string &second_arg){
        std::string answer =  "command: " + command + " id: " + std::to_string(id) + " first_arg: " + first_arg + " second_arg: " + second_arg;
        write_possible_sequence(answer);
    }


private:

    std::string parse_user_input_and_make_command(const std::string &message){
        std::istringstream iss(message);
        std::string command;
        std::string first_arg;
        std::string second_arg;

        if (iss)
            iss >> command;
        if (iss)
            iss >> first_arg;
        std::getline(iss, second_arg);

        std::string answer =  "command: " + command + " id: " + std::to_string(id) + " first_arg: " + first_arg + " second_arg: " + second_arg;

        return answer;
    }

    void parse_answer_and_reply(const std::string &message) override {

        if (message.find(std::string("ping")) == 0) {
            //write_possible_sequence(parse_user_input_and_make_command("send received_ping_back"));
        }
        else if (message.find(std::string("id")) == 0){
            sscanf(message.c_str(), "id: %d", &id);
        }
        else if (message.find(std::string("command")) == 0) {
            parse_command_and_do_something(message);
        }


    }
    void parse_command_and_do_something(const std::string& message) {
        std::istringstream iss(message);
        std::string command;
        std::string first_arg;
        std::string second_arg;
        std::string trash;
        int id = -1;

        if (iss)
            iss >> trash;
        if (iss)
            iss >> command;
        if (iss)
            iss >> trash;
        if (iss)
            iss >> id;
        if (iss)
            iss >> trash;
        if (iss)
            iss >> first_arg;
        if (iss)
            iss >> trash;
        std::getline(iss, second_arg);

        //printf("got command: %s\n", command.c_str());

        if (command.find(std::string("print")) == 0) {
            printf("%s\n", second_arg.c_str());
        }
    }
};



#endif //DFS_CLIENT_SERVER_H
