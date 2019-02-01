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

Server_Message:: Server_Message(char* data, bool final){
    body_length(std::strlen(data));
    std::memcpy(data_ + header_length, data, body_length_);
    encode_header(final);
}

Server_Message:: Server_Message(std::string data, bool final){
    body_length(data.length());
    std::memcpy(data_ + header_length, data.c_str(), body_length_);
    encode_header(final);
}

void Server_Message::make_message(const char * message, bool final){
    body_length(std::strlen(message));
    std::memcpy(data_ + header_length, message, body_length_);
    encode_header(final);
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

std::string Server_Message::read(){
    return std::string(body(), 0, body_length_);
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

int Server_Message::decode_header()
    {
        char header[header_length + 1] = "";
        std::strncat(header, data_, header_length);
        body_length_ = std::atoi(header);
        if (body_length_ > max_body_length)
        {
            body_length_ = 0;
            return 0;
        }
        if (header[4] == 'f'){
            return 2;
        }
        return 1;
    }

void Server_Message::encode_header(bool final)
    {
        char header[header_length + 1] = "";
        if (final)
            std::sprintf(header, "%4df", body_length_);
        else
            std::sprintf(header, "%4d", body_length_);
        std::memcpy(data_, header, header_length);
    }