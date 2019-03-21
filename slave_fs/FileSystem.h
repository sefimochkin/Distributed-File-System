//
// Created by Сергей Ефимочкин on 18.04.2018.
//

#ifndef FS_FILESYSTEM_H
#define FS_FILESYSTEM_H
#include "superblock.h"

typedef
struct FS_Handler
        FS_Handler;

void * get_memory_for_filesystem();

void create_filesystem(char* filesystem);
void open_filesystem(char* file_system_name, char* filesystem);
char* save_filesystem(char* file_system_name, char* filesystem);
char* close_filesystem(char* file_system_name, char* filesystem);

#endif //FS_FILESYSTEM_H
