//
// Created by Сергей Ефимочкин on 2019-01-28.
//

#include <cstdlib>
#include <boost/asio.hpp>
#include <iostream>
#include "Client_Server.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
    try
    {

        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        auto endpoint_iterator = resolver.resolve({ boost::asio::ip::address::from_string("127.0.0.1"), 8000 });
        Client_Server c(io_service, endpoint_iterator);

        io_service.run();

        /*
        char line[Server_Message::max_body_length + 1];
        while (std::cin.getline(line, Server_Message::max_body_length + 1))
        {
            Server_Message msg;
          msg.body_length(std::strlen(line));
          std::memcpy(msg.body(), line, msg.body_length());
          msg.encode_header();
          c.write(msg);
        }
        */
        //c.close();
//    t.join();
        printf("Finished Client");

    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}