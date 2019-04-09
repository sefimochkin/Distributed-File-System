//
// Created by Сергей Ефимочкин on 2019-01-30.
//

#ifndef DFS_BASE_SERVER_GROUP_H
#define DFS_BASE_SERVER_GROUP_H

#include "../utils/Server_Message.h"
#include "Inter_Server_Session.h"
#include <unordered_map>
#include <atomic>

class Server_Participant;
typedef std::shared_ptr<Server_Participant> server_participant_ptr;


class Server_Group
{
public:

    Server_Group();

    void join(server_participant_ptr server);

    void leave(server_participant_ptr server);

    void deliver(const std::string message);

    int len();

    void ping();

    void parse_possible_sequence(server_participant_ptr server, std::string message, int header_code);
    
    virtual void parse(server_participant_ptr server, std::string message) = 0;

    virtual void send_command(std::string message) = 0;

    virtual void do_something_on_leave(int server_id) = 0;

    virtual std::string get_fs_info() = 0;

    void add_pointer_to_other_group(Server_Group *other_group){
        other_group_ = other_group;
    }


    virtual ~Server_Group() = default;

protected:
    std::unordered_map<int, server_participant_ptr> participants_;
    enum { max_recent_msgs = 100 };
    chat_message_queue recent_msgs_;
    std::atomic<int> current_id = {0};
    std::unordered_map<server_participant_ptr, std::shared_ptr<std::string>> continuous_messages;
    Server_Group *other_group_;

};
#endif //DFS_BASE_SERVER_GROUP_H
