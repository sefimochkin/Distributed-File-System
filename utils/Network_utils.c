//
// Created by Сергей Ефимочкин on 10.05.2018.
//
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "Network_utils.h"

int safe_parse_input(int sock, char* input, short first_size, short second_size, short third_size,
                     char* first_arg, char* second_arg, char* third_arg) {
    memset(first_arg, '\0', first_size + 1);
    memset(second_arg, '\0', second_size + 1);
    memset(third_arg, '\0', third_size + 1);

    short k = 0;
    short sizes[3] = {first_size, second_size, third_size};
    char* args[3] = {first_arg, second_arg, third_arg};
    int current_arg = 0;
    short j = 0;
    while ((k < strlen(input)) && (current_arg < 3) && (input[k] != '\n')) {
        while ((j < sizes[current_arg]) && (input[k] != ' ') && (k < strlen(input)) && (input[k] != '\n')) {
            args[current_arg][j] = input[k];
            j++;
            k++;
        }

        if ((j == sizes[current_arg]) && (input[k] != ' ') && (k < strlen(input)) && (input[k] != '\n')) {
            send_answer(sock, "Argument too long!");
            break;
        }
        j = 0;
        current_arg++;
        k++;
    }


    return current_arg;
}

void send_answer(int sock, char* answer){
    char buf[16];
    sprintf(buf, "%i", strlen(answer));
    send(sock, buf, 16, 0);
    send(sock, answer, strlen(answer), 0);
}

char* get_answer(int sock){
    char buf[16];
    recv(sock, buf, 16, 0);
    int size_of_answer;
    sscanf(buf, "%i", &size_of_answer);

    char *answer = malloc(sizeof(char) * (size_of_answer + 1));
    memset(answer, 0, (size_of_answer + 1));

    recv(sock, answer, (size_t)size_of_answer, 0);
    return answer;
}
