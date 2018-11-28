//
// Created by Сергей Ефимочкин on 21.11.2018.
//

#ifndef DFS_SERVER_MESSAGE_H
#define DFS_SERVER_MESSAGE_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>

class Server_Message
{
public:
    enum { header_length = 4 };
    enum { max_body_length = 512 };

    Server_Message();
    Server_Message(char* data);

    void make_message(char * message);

    const char* data() const;

    char* data();

    std::size_t length() const;

    const char* body() const;

    char* body();

    std::size_t body_length() const;

    void body_length(std::size_t new_length);

    bool decode_header();

    void encode_header();


private:
    char data_[header_length + max_body_length];
    std::size_t body_length_;
};
typedef std::deque<Server_Message> chat_message_queue;

#endif //DFS_SERVER_MESSAGE_H
