//
// Created by Сергей Ефимочкин on 18.04.2018.
//

#ifndef FS_FILESYSTEM_H
#define FS_FILESYSTEM_H
#include "superblock.h"

typedef
struct FS_Handler
        FS_Handler;

void * get_memory_for_filesystem(int number_of_blocks);

void create_filesystem(char* filesystem, int number_of_blocks);
void open_filesystem(char* file_system_name, char* filesystem, int number_of_blocks);
char* save_filesystem(char* file_system_name, char* filesystem);
char* close_filesystem(char* file_system_name, char* filesystem);


int get_size_of_data_in_blocks(struct superblock *sb, int size_of_data);

#endif //FS_FILESYSTEM_H
