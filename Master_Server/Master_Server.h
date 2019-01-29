//
// Created by Сергей Ефимочкин on 11.11.2018.
//

#ifndef FS_Master_Server_H
#define FS_Master_Server_H

#include <boost/asio.hpp>
#include "Slaves_Group.h"
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Inter_Server_Session.h"
//#include <boost/lockfree/queue.hpp>
#include <boost/thread/thread.hpp>
#include <queue>
#include <mutex>
#include <algorithm>



using boost::asio::ip::tcp;


//void send_messages_from_queue(server_participant_ptr server, Server_Message message);
//void send_messages_from_queue(std::queue<Server_And_Message> *output_queue, std::mutex *output_queue_mutex);


//void receive_messages(std::vector<server_participant_ptr> slave_servers, boost::asio::io_service& io_service);

class Master_Server
{
public:
    Master_Server(boost::asio::io_service& io_service,
                  const tcp::endpoint& endpoint)
            : acceptor_(io_service, endpoint),
              socket_(io_service), ping_timer(io_service, boost::posix_time::seconds(ping_period)),
              strand(io_service), io_service_(io_service)
    {
        boost::asio::io_service::work work(io_service);
        slaves_group = Slaves_Group();
        clients_group = Slaves_Group();
        for (std::size_t i = 0; i < number_of_worker_threads; ++i)
            worker_threads.create_thread(boost::bind(&boost::asio::io_service::run, &io_service));
        ping_timer.async_wait(strand.wrap(boost::bind(&Master_Server::ping, this)));
        do_accept();

    }

    void ping(){

        slaves_group.ping();

        printf("Slave Server#: %d\n", slaves_group.len());

        ping_timer.expires_at(ping_timer.expires_at() + boost::posix_time::seconds(ping_period));
        ping_timer.async_wait(strand.wrap(boost::bind(&Master_Server::ping, this)));
    }

private:
    void do_accept(){
        acceptor_.async_accept(socket_,
                               [this](boost::system::error_code ec)
                               {
                                   if (!ec)
                                   {
                                       printf("connected!");
                                       std::make_shared<Inter_Server_Session>(std::move(socket_), slaves_group, clients_group)->start();
                                   }
                                   else{

                                       printf("%s", ec.message().c_str());
                                   }

                                   do_accept();
                               });
    }

    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    Slaves_Group slaves_group;
    Slaves_Group clients_group;
    int ping_period = 60;
    boost::asio::deadline_timer ping_timer;
    boost::asio::io_service::strand strand;
    boost::thread_group worker_threads;
    int number_of_worker_threads = 4;


};

#endif //FS_Master_Server_H
