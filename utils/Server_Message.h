//
// Created by Сергей Ефимочкин on 21.11.2018.
//

#ifndef DFS_SERVER_MESSAGE_H
#define DFS_SERVER_MESSAGE_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <queue>
#include <string>
#include <iostream>

class Server_Message
{
public:
    enum { header_length = 5 };
    enum { max_body_length = 5};

    Server_Message();
    Server_Message(char* data, bool final = true);
    Server_Message(std::string data, bool final = true);


    void make_message(const char * message, bool final = true);


    const char* data() const;

    char* data();

    std::size_t length() const;

    const char* body() const;

    char* body();

    std::string read();

    std::size_t body_length() const;

    void body_length(std::size_t new_length);

    int decode_header();

    void encode_header(bool final=true);

    static std::queue<Server_Message> make_messages(const std::string message){
            std::queue<Server_Message> output;
            unsigned long i = max_body_length;
            //std::cout << "starting making: ";
            while (i < message.length()){
                std::string message_part(message, i - max_body_length, i);
                Server_Message msg;
                msg.make_message(message_part.c_str(), false);
                //std::cout << message_part;
                output.push(msg);
                i += max_body_length;
            }
            std::string message_part(message, i - max_body_length, message.length());
            //std::cout << message_part;
            //std::cout << '\n';

            Server_Message msg;
            msg.make_message(message_part.c_str());
            output.push(msg);
            return output;
        }


private:
    char data_[header_length + max_body_length];
    std::size_t body_length_;
};
typedef std::deque<Server_Message> chat_message_queue;

#endif //DFS_SERVER_MESSAGE_H
