//
// Created by Сергей Ефимочкин on 11.11.2018.
//
#include "Master_Server.h"
#include <random>

Master_Server::Master_Server(boost::asio::io_service& io_service,const tcp::endpoint& endpoint)
    : acceptor_(io_service, endpoint),
    socket_(io_service), ping_timer(io_service, boost::posix_time::seconds(ping_period)),
    strand(io_service), io_service_(io_service)
    {
        boost::asio::io_service::work work(io_service);
        clients_group.add_pointer_to_other_group(&slaves_group);
        slaves_group.add_pointer_to_other_group(&clients_group);

        for (std::size_t i = 0; i < number_of_worker_threads; ++i)
            worker_threads.create_thread(boost::bind(&boost::asio::io_service::run, &io_service));
        ping_timer.async_wait(strand.wrap(boost::bind(&Master_Server::ping, this)));
        do_accept();

}

void Master_Server::ping(){
    slaves_group.ping();
    //clients_group.ping();

    printf("Slave Server#: %d\n", slaves_group.len());

    ping_timer.expires_at(ping_timer.expires_at() + boost::posix_time::seconds(ping_period));
    ping_timer.async_wait(strand.wrap(boost::bind(&Master_Server::ping, this)));
}

void Master_Server::do_accept(){
    acceptor_.async_accept(socket_,
                           [this](boost::system::error_code ec)
                           {
                               if (!ec)
                               {
                                   printf("connected!");
                                   std::make_shared<Initializing_ISS>(std::move(socket_), slaves_group, clients_group)->start();
                               }
                               else{

                                   printf("%s", ec.message().c_str());
                               }

                               do_accept();
                           });
}