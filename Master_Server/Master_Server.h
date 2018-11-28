//
// Created by Сергей Ефимочкин on 11.11.2018.
//

#ifndef FS_Master_Server_H
#define FS_Master_Server_H

#include <boost/asio.hpp>
#include "Server_Group.h"
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Inter_Server_Session.h"
//#include <boost/lockfree/queue.hpp>
#include <boost/thread/thread.hpp>
#include <queue>
#include <mutex>
#include <algorithm>
#include <random>



//std::random_device rd;     // only used once to initialise (seed) engine
//std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)



struct Server_And_Message{

    Server_And_Message(server_participant_ptr server_, Server_Message message_){
        server = server_;
        message = message_;
    }

    server_participant_ptr server;
    Server_Message message;
};


using boost::asio::ip::tcp;


void send_messages_from_queue(server_participant_ptr server, Server_Message message);
//void send_messages_from_queue(std::queue<Server_And_Message> *output_queue, std::mutex *output_queue_mutex);

/*
void receive_messages(std::set<server_participant_ptr> slave_servers){
    if (!slave_servers.empty()) {
        std::uniform_int_distribution<int> uni(0, slave_servers.size() - 1);
        int random_slave = uni(rng);

        auto it(slave_servers.begin());
        advance(it, random_slave);

    }

}*/

class Master_Server
{
public:
    Master_Server(boost::asio::io_service& io_service,
                  const tcp::endpoint& endpoint)
            : acceptor_(io_service, endpoint),
              socket_(io_service), ping_timer(io_service, boost::posix_time::seconds(ping_period)),
              strand(io_service), io_service_(io_service)
    {
        slaves_group = Server_Group();
        for (std::size_t i = 0; i < number_of_worker_threads; ++i)
            worker_threads.create_thread(boost::bind(&boost::asio::io_service::run, &io_service));
        ping_timer.async_wait(strand.wrap(boost::bind(&Master_Server::ping, this)));
        do_accept();

    }

    void ping(){
        Server_Message msg = Server_Message();
        msg.make_message("ping");

        for (const auto &participant : slaves_group.participants_) {
            io_service_.post(boost::bind(&send_messages_from_queue, participant, msg));
            //Server_And_Message *package = new Server_And_Message(participant, msg);
            //output_queue_mutex.lock();
            //output_queue.push(*package);
            //output_queue_mutex.unlock();
        }
        //slaves_group.deliver(msg);

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
                                       std::make_shared<Inter_Server_Session>(std::move(socket_), slaves_group)->start();
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
    Server_Group slaves_group;
    int ping_period = 10;
    boost::asio::deadline_timer ping_timer;
    boost::asio::io_service::strand strand;
    boost::thread_group worker_threads;
    int number_of_worker_threads = 4;


    std::mutex output_queue_mutex;
    std::queue<Server_And_Message> output_queue;

};

#endif //FS_Master_Server_H
