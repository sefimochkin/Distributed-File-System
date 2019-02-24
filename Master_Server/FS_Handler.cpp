//
// Created by Сергей Ефимочкин on 2019-02-23.
//

#include "FS_Handler.h"
//#include "../fs/FileSystem.h"


std::string FS_Handler::do_command(int client_id, const std::string& command, const std::string& first_arg, const std::string& second_arg){

    auto it = clients_cur_directories.find(client_id);

    if(it == clients_cur_directories.end())
    {
        clients_cur_directories[client_id] = &root;
    }

    int number_of_arguments = 0;
    if (command.length() > 0)
        number_of_arguments++;
    if (first_arg.length() > 0)
        number_of_arguments++;
    if (second_arg.length() > 0)
        number_of_arguments++;

    std::string answer;

    if(command.find(std::string("ls")) == 0){
        char* output = ls(sb, *clients_cur_directories[client_id]);
        answer = std::string(output);
        if((*clients_cur_directories[client_id])->number_of_files_in_directory > 0)
            free(output);
    }

    else if (command.find(std::string("mkdir")) == 0){
        if(number_of_arguments == 1)
            answer =  std::string("Not sufficient arguments!");
        else {
            answer = mkdirf(sb, first_arg.c_str(), *clients_cur_directories[client_id]);
        }
    }

    else if (command.find(std::string("rmdir")) == 0){
        if(number_of_arguments == 1)
            answer =  std::string("Not sufficient arguments!");
        else {
            answer = rm_dir(sb, first_arg.c_str(), *clients_cur_directories[client_id]);
        }
    }

    else if (command.find(std::string("touch")) == 0){
        if(number_of_arguments < 3)
            answer =  std::string("Not sufficient arguments!");
        else {
            answer = touch(sb, first_arg.c_str(), second_arg.c_str(), *clients_cur_directories[client_id]);
        }
    }

    else if (command.find(std::string("rm")) == 0){
        if(number_of_arguments == 1)
            answer =  std::string("Not sufficient arguments!");
        else {
            answer = rm(sb, first_arg.c_str(), *clients_cur_directories[client_id]);
        }
    }

    else if (command.find(std::string("read")) == 0){
        if(number_of_arguments == 1)
            answer =  std::string("Not sufficient arguments!");
        else {
            short failed = 0;
            char *output = read_file(sb, first_arg.c_str(), *clients_cur_directories[client_id], &failed);
            answer =  std::string(output);
            if(failed != 1) {
                free(output);
            }

        }
    }

    else if (command.find(std::string("cd")) == 0){
        if(number_of_arguments == 1)
            answer =  std::string("Not sufficient arguments!");
        else {
            answer = cd(sb, first_arg.c_str(), clients_cur_directories[client_id]);
        }
    }

    else if (command.find(std::string("import")) == 0){
        if(number_of_arguments < 3)
            answer =  std::string("Not sufficient arguments!");
        else {
            answer = touch(sb, first_arg.c_str(), second_arg.c_str(), *clients_cur_directories[client_id]);
        }
    }

    else if (command.find(std::string("export")) == 0){
        if(number_of_arguments < 3)
            answer =  std::string("Not sufficient arguments!");
        else {
            short failed = 0;
            char *output = read_file(sb, first_arg.c_str(), *clients_cur_directories[client_id], &failed);
            answer =  std::string(output);
            if(failed != 1) {
                free(output);
            }

        }
    }

    else if (command.find(std::string("save")) == 0){
        answer = save_filesystem(name, (char *) filesystem);
    }

    else if (command.find(std::string("help")) == 0){
        answer =  std::string("ls to list files in current directory\ncd $name$ to go to directory named $name$\n"
                          "mkdir $name$ to create directory named $name$\ntouch $name$ $file text$ to create a file "
                          "named $name$ with text of file $file text$\nrmdir $name$ to delete directory named $name$ "
                          "and all files in it\nrm $name$ to delete file named $name$ from directory\n"
                          "read $name$ to print text of file named $name$\nimport $inner name$ $outer name$ to import "
                          "file named $outer name$ from computer's file system and save it as file named $inner name$ in "
                          "this file system\nexport $inner name$ $outer name$ to export file named $inner name$ into "
                          "computer's file system as a file named $outername$\nsave to save all changes made in "
                          "the filesystem\nexit to save and exit");
    }

    else if (command.find(std::string("exit")) == 0) {
        answer =  std::string("");
    }
/*
    else if (command.find(std::string("stop")) == 0){
        running = 0;
        answer =  std::string("");
    }
*/
    else { //in case of unknown command or a misspell
        answer =  std::string("");
    }
    return answer;

}
