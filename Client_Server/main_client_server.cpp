//
// Created by Сергей Ефимочкин on 2019-01-28.
//

#include <cstdlib>
#include <boost/asio.hpp>
#include <iostream>
#include "Client_Server.h"
#include <sstream>
#include <fstream>
#include "../utils/Server_Message.h"


using boost::asio::ip::tcp;
using namespace std;

int main(int argc, char* argv[])
{
    try
    {
        boost::asio::io_service io_service;


        tcp::resolver resolver(io_service);
        auto endpoint_iterator = resolver.resolve({ boost::asio::ip::address::from_string("127.0.0.1"), 8000 });
        //84.201.143.118
        Client_Server client(io_service, endpoint_iterator);

        std::thread t([&io_service](){ io_service.run(); });

        std::string command;
        std::string first_argument;
        std::string second_argument;
        std::string input_line;

        std::getline(std::cin, input_line);
        std::istringstream iss(input_line);
        iss >> command;
        iss >> first_argument;
        std::getline(iss, second_argument);
        if (second_argument.length() > 0)
            second_argument = second_argument.substr(1);

        while(command.find("exit") != 0){
            client.gain_control();
            client.input_message(command, first_argument, second_argument);

            command = "";
            first_argument = "";
            second_argument = "";

            std::getline(std::cin, input_line);
            std::istringstream iss(input_line);
            iss >> command;
            iss >> first_argument;
            std::getline(iss, second_argument);
            if (second_argument.length() > 0)
                second_argument = second_argument.substr(1);

            //int number_of_arguments = std::sscanf(input_line.c_str(), "%s %s %s", command.c_str(),
            //                                      first_argument.c_str(), second_argument.c_str());
        }


        printf("Finished Client");


    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}