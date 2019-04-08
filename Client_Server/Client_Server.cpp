//
// Created by Сергей Ефимочкин on 2019-01-28.
//

#include "Client_Server.h"


Client_Server::Client_Server(boost::asio::io_service& io_service,
    tcp::resolver::iterator endpoint_iterator) :
    Server(io_service, endpoint_iterator) {
        Server_Message msg = Server_Message();
        msg.make_message("client");
        write(msg);
    }


/*
    void send_command(std::string &command, std::string &first_argument, std::string &second_argument){
        io_service_.post(
                [this, command, first_argument, second_argument]()
                {
                    std::cout << command;

                });
    }
*/
void Client_Server::input_message(const std::string &command, std::string &first_arg, std::string &second_arg){
    if (command.find("import") == 0) {
        std::ifstream instream(first_arg);
        std::stringstream buffer;
        buffer << instream.rdbuf();
        first_arg = second_arg;
        second_arg = buffer.str();
    }

    if (command.find("export") == 0) {
        waiting_on_export = true;
        name_of_export_file = second_arg;
        std::string answer =  "command: " + command + " id: " + std::to_string(id) + " first_arg: " + first_arg + " second_arg: " + second_arg;
        write_possible_sequence(answer);
    }

    else if (command.find("read") == 0) {
        std::string answer =  "command: " + command + " id: " + std::to_string(id) + " first_arg: " + first_arg + " second_arg: " + second_arg;
        write_possible_sequence(answer);
    }

    else {
        std::string answer = "command: " + command + " id: " + std::to_string(id) + " first_arg: " + first_arg +
                             " second_arg: " + second_arg;
        write_possible_sequence(answer);
        waiting_on_read.unlock();
    }
}

void Client_Server::gain_control(){
    waiting_on_read.lock();
}


std::string Client_Server::parse_user_input_and_make_command(const std::string &message){
    std::istringstream iss(message);
    std::string command;
    std::string first_arg;
    std::string second_arg;

    if (iss)
        iss >> command;
    if (iss)
        iss >> first_arg;

    if (iss.tellg() > 0)
        second_arg = iss.str().substr(iss.tellg());
    else
        std::getline(iss, second_arg);
    if (second_arg.length() > 0)
        second_arg = second_arg.substr(1);

    std::string answer =  "command: " + command + " id: " + std::to_string(id) + " first_arg: " + first_arg + " second_arg: " + second_arg;

    return answer;
}

void Client_Server::parse_answer_and_reply(const std::string &message) {

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

void Client_Server::parse_command_and_do_something(const std::string& message) {
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

    if (iss.tellg() > 0)
        second_arg = iss.str().substr(iss.tellg());
    else
        std::getline(iss, second_arg);
    if (second_arg.length() > 0)
        second_arg = second_arg.substr(1);

    if (command.find(std::string("print")) == 0) {
        printf("%s\n", second_arg.c_str());
    }
    else if (command.find("error_print") == 0){
        printf("%s\n", second_arg.c_str());
        waiting_on_read.unlock();
    }
    else if (command.find(std::string("read_whole")) == 0) {
        if (waiting_on_export) {
            std::ofstream out(name_of_export_file);
            out << second_arg;
            out.close();
            waiting_on_export = false;
            waiting_on_read.unlock();
        }
        else {
            printf("READ: %s\n", second_arg.c_str());
            waiting_on_read.unlock();
        }
    }

}