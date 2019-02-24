//
// Created by Сергей Ефимочкин on 2019-01-28.
//

#include <cstdlib>
#include <boost/asio.hpp>
#include <iostream>
#include "Client_Server.h"
#include <sstream>
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
        Client_Server client(io_service, endpoint_iterator);

        std::thread t([&io_service](){ io_service.run(); });

        std::string command;
        std::string first_argument;
        std::string second_argument;

        std::cin >> command;
        std::cin >> first_argument;
        std::getline(std::cin, second_argument);


        while(command.find("finish") != 0){
            client.input_message(command, first_argument, second_argument);

            std::cin >> command;
            std::cin >> first_argument;
            std::getline(std::cin, second_argument);
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