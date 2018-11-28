//
// Created by Сергей Ефимочкин on 10.05.2018.
//

#ifndef FILE_SYSTEM_NETWORK_UTILS_H
#define FILE_SYSTEM_NETWORK_UTILS_H

int safe_parse_input(int sock, char* input, short first_size, short second_size, short third_size,
                     char* first_arg, char* second_arg, char* third_arg);
void send_answer(int sock, char* answer);
char* get_answer(int sock);

#endif //FILE_SYSTEM_NETWORK_UTILS_H
