//
// Created by Сергей Ефимочкин on 11.11.2018.
//

#include "Master_Server.h"
#include <random>

/*
std::random_device rd;     // only used once to initialise (seed) engine
std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
void send_messages_from_queue(server_participant_ptr server, Server_Message message){

        server->deliver(message);
}*/
/*
void receive_messages(std::vector<server_participant_ptr> slave_servers, boost::asio::io_service& io_service){
    if (!slave_servers.empty()) {
        std::uniform_int_distribution<int> uni(0, slave_servers.size() - 1);
        int random_slave = uni(rng);

        //auto it(slave_servers.begin());
        //advance(it, random_slave);
        Server_Message answer = (*slave_servers[random_slave]).get_message();
        printf("%s", answer.body());
    }
    //receive_messages(slave_servers);
    //io_service.post(boost::bind(&receive_messages, slave_servers, io_service));


}*/

/*
void send_messages_from_queue(std::queue<Server_And_Message> *output_queue, std::mutex *output_queue_mutex){
    bool continue_working = true;
    if (!output_queue->empty()){

        output_queue_mutex->lock();
        Server_And_Message package = output_queue->front();
        output_queue->pop();
        output_queue_mutex->unlock();

        package.server->deliver(package.message);
        if (strcmp(package.message.data(), "stop") != 0)
            continue_working = false;
    }
    if (continue_working)
        send_messages_from_queue(output_queue, output_queue_mutex);
}
 */