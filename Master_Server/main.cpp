

#include <iostream>
#include <boost/asio.hpp>
#include "Master_Server.h"
#include <string>

int main() {

    boost::asio::io_service io_service;


    tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080);
    //10.129.0.24
    Master_Server main_server(io_service, endpoint);
    try {
        io_service.run();
    } catch( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}