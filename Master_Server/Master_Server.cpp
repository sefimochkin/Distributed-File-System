//
// Created by Сергей Ефимочкин on 11.11.2018.
//

#include "Master_Server.h"
void send_messages_from_queue(server_participant_ptr server, Server_Message message){

        server->deliver(message);
}

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