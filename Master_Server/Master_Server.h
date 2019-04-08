//
// Created by Сергей Ефимочкин on 11.11.2018.
//

#ifndef FS_Master_Server_H
#define FS_Master_Server_H

#include <boost/asio.hpp>
#include "Slaves_Group.h"
#include "Clients_Group.h"
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Initializing_ISS.h"
//#include <boost/lockfree/queue.hpp>
#include <boost/thread/thread.hpp>
#include <queue>
#include <mutex>
#include <algorithm>



using boost::asio::ip::tcp;



class Master_Server
{
public:
    Master_Server(boost::asio::io_service& io_service, const tcp::endpoint& endpoint);

    void ping();

private:
    void do_accept();

    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    Slaves_Group slaves_group;
    Clients_Group clients_group;
    int ping_period = 60;
    boost::asio::deadline_timer ping_timer;
    boost::asio::io_service::strand strand;
    boost::thread_group worker_threads;
    int number_of_worker_threads = 1; //that's because i have little experience in concurrency and i fucked up with it!

};

#endif //FS_Master_Server_H
