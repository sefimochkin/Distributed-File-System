//
// Created by Сергей Ефимочкин on 21.11.2018.
//

#include "Server_Message.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>


Server_Message::Server_Message()
        : body_length_(0)
        {}

void Server_Message::make_message(char * message){
    body_length(std::strlen(message));
    std::memcpy(data_ + header_length, message, body_length_);
    encode_header();
}


const char* Server_Message::data() const
    {
        return data_;
    }

char* Server_Message::data()
    {
        return data_;
    }

std::size_t Server_Message::length() const
    {
        return header_length + body_length_;
    }

const char* Server_Message::body() const
    {
        return data_ + header_length;
    }

char* Server_Message::body()
    {
        return data_ + header_length;
    }

std::size_t Server_Message::body_length() const
    {
        return body_length_;
    }

void Server_Message::body_length(std::size_t new_length)
    {
        body_length_ = new_length;
        if (body_length_ > max_body_length)
            body_length_ = max_body_length;
    }

bool Server_Message::decode_header()
    {
        char header[header_length + 1] = "";
        std::strncat(header, data_, header_length);
        body_length_ = std::atoi(header);
        if (body_length_ > max_body_length)
        {
            body_length_ = 0;
            return false;
        }
        return true;
    }

void Server_Message::encode_header()
    {
        char header[header_length + 1] = "";
        std::sprintf(header, "%4d", body_length_);
        std::memcpy(data_, header, header_length);
    }