//
// Created by Сергей Ефимочкин on 18.04.2018.
//

#ifndef FS_FILESYSTEM_H
#define FS_FILESYSTEM_H
#include "inode.h"
#include "superblock.h"

void * get_memory_for_filesystem();

struct inode * create_filesystem(char* filesystem);
struct inode * open_filesystem(char* file_system_name, char* filesystem);
char* save_filesystem(char* file_system_name, char* filesystem);
char* close_filesystem(char* file_system_name, char* filesystem);

char* ls(struct superblock *sb, struct inode* directory);
char* mkdirf(struct superblock *sb, const char* name, struct inode* directory);
char* rm_dir(struct superblock *sb, const char* name, struct inode* directory);
char* rm(struct superblock *sb, const char* name, struct inode* directory);
char* touch(struct superblock *sb, const char* name, const char* input, struct inode* directory);
char* read_file(struct superblock *sb, const char* name, struct inode* directory, short * failed);
char* cd(struct superblock *sb, const char*name, struct inode** current_directory);

#endif //FS_FILESYSTEM_H
