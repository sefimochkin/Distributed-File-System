//
// Created by Сергей Ефимочкин on 18.04.2018.
//

#ifndef FS_FILESYSTEM_H
#define FS_FILESYSTEM_H
#include "inode.h"
#include "superblock.h"

void * get_memory_for_filesystem();

struct inode * create_filesystem(char* filesystem);
struct inode * open_filesystem(char* file_system_name, char* filesystem, int sock);
void save_filesystem(char* file_system_name, char* filesystem, int sock);
void close_filesystem(char* file_system_name, char* filesystem, int sock);

char* ls(struct superblock *sb, struct inode* directory);
void mkdir(struct superblock *sb, char* name, struct inode* directory, int sock);
void rm_dir(struct superblock *sb, char* name, struct inode* directory, int sock);
void rm(struct superblock *sb, char* name, struct inode* directory, int sock);
void touch(struct superblock *sb, char* name, char* input, struct inode* directory, int sock);
char* read_file(struct superblock *sb, char* name, struct inode* directory, int sock);
void cd(struct superblock *sb, char*name, struct inode** current_directory, int sock);
void import_file(struct superblock *sb, char* inner_name, char* outer_name, struct inode* directory, int sock);
void export_file(struct superblock *sb, char* inner_name, char* outer_name, struct inode* directory, int sock);

#endif //FS_FILESYSTEM_H
