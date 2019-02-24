//
// Created by Сергей Ефимочкин on 10.05.2018.
//


#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "fs/FileSystem.h"
#include "utils/Network_utils.h"
#include "FS_Server.h"

#define COMMAND_SIZE 32
#define FIRST_ARG_SIZE 128
#define SECOND_ARG_SIZE 1024
#define INPUT_SIZE 1186
/*
void Server_run() {
    //daemon(0, 0);
    int sock, listener;
    struct sockaddr_in addr;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8901);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(listener, (struct sockaddr *) &addr, sizeof(addr));
    listen(listener, 1);
    _Bool running = 1;

    while (running == 1) {
        sock = accept(listener, NULL, NULL);

        char * path = get_answer(sock);
        char* name = malloc(strlen(path) +3);
        strncpy(name, "./", 2);
        strncpy(&name[2], path, strlen(path) + 1);

        void* filesystem = get_memory_for_filesystem();

        char command[COMMAND_SIZE + 1]; // +1 for terminating symbol
        char first_argument[FIRST_ARG_SIZE + 1];
        char second_argument[SECOND_ARG_SIZE + 1];
        char input_line[INPUT_SIZE + 1];

        struct inode* root = open_filesystem(name, (char*)filesystem, sock);

        struct superblock* sb = (struct superblock *) filesystem;

        struct inode** current_directory = &root;
        while (recv(sock, input_line, INPUT_SIZE, 0) != 0) {
            int number_of_arguments = safe_parse_input(sock, input_line, COMMAND_SIZE, FIRST_ARG_SIZE, SECOND_ARG_SIZE,
                                                       command, first_argument, second_argument);

            if(strcmp(command, "ls") == 0){
                char* output = ls(sb, *current_directory);
                send_answer(sock, output);
                if((*current_directory)->number_of_files_in_directory > 0)
                    free(output);
            }

            else if (strcmp(command, "mkdir") == 0){
                if(number_of_arguments == 1)
                    send_answer(sock, "Not sufficient arguments!");
                else {
                    mkdir(sb, first_argument, *current_directory, sock);
                    send_answer(sock, "");
                }

            }

            else if (strcmp(command, "rmdir") == 0){
                if(number_of_arguments == 1)
                    send_answer(sock, "Not sufficient arguments!");
                else {
                    rm_dir(sb, first_argument, *current_directory, sock);
                    send_answer(sock, "");
                }
            }

            else if (strcmp(command, "touch") == 0){
                if(number_of_arguments < 3)
                    send_answer(sock, "Not sufficient arguments!");
                else {
                    touch(sb, first_argument, second_argument, *current_directory, sock);
                    send_answer(sock, "");
                }
            }

            else if (strcmp(command, "rm") == 0){
                if(number_of_arguments == 1)
                    send_answer(sock, "Not sufficient arguments!");
                else {
                    rm(sb, first_argument, *current_directory, sock);
                    send_answer(sock, "");

                }
            }

            else if (strcmp(command, "read") == 0){
                if(number_of_arguments == 1)
                    send_answer(sock, "Not sufficient arguments!");
                else {
                    char *output = read_file(sb, first_argument, *current_directory, sock);
                    if(output != NULL) {
                        send_answer(sock, output);
                        free(output);
                    }

                }
            }

            else if (strcmp(command, "cd") == 0){
                if(number_of_arguments == 1)
                    send_answer(sock, "Not sufficient arguments!");
                else {
                    cd(sb, first_argument, current_directory, sock);
                    send_answer(sock, "");
                }
            }

            else if (strcmp(command, "import") == 0){
                if(number_of_arguments < 3)
                    send_answer(sock, "Not sufficient arguments!");
                else {
                    import_file(sb, first_argument, second_argument, *current_directory, sock);
                    send_answer(sock, "");
                }
            }

            else if (strcmp(command, "export") == 0){
                if(number_of_arguments < 3)
                    send_answer(sock, "Not sufficient arguments!");
                else {
                    export_file(sb, first_argument, second_argument, *current_directory, sock);
                    send_answer(sock, "");
                }
            }

            else if (strcmp(command, "save") == 0){
                save_filesystem(name, filesystem, sock);
                send_answer(sock, "");
            }

            else if (strcmp(command, "help") == 0){
                send_answer(sock, "ls to list files in current directory\ncd $name$ to go to directory named $name$\n"
                                  "mkdir $name$ to create directory named $name$\ntouch $name$ $file text$ to create a file "
                                  "named $name$ with text of file $file text$\nrmdir $name$ to delete directory named $name$ "
                                  "and all files in it\nrm $name$ to delete file named $name$ from directory\n"
                                  "read $name$ to print text of file named $name$\nimport $inner name$ $outer name$ to import "
                                  "file named $outer name$ from computer's file system and save it as file named $inner name$ in "
                                  "this file system\nexport $inner name$ $outer name$ to export file named $inner name$ into "
                                  "computer's file system as a file named $outername$\nsave to save all changes made in "
                                  "the filesystem\nexit to save and exit");
            }

            else if (strcmp(command, "exit") == 0) {
                send_answer(sock, "");
                break;
            }

            else if (strcmp(command, "stop") == 0){
                running = 0;
                send_answer(sock, "");
                break;
            }

            else { //in case of unknown command or a misspell
                send_answer(sock, "");
            }
        }
        close(sock);
        close_filesystem(name, filesystem, sock);
        free(path);
        free(name);
    }

}
*/