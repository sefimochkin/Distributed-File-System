//
// Created by Сергей Ефимочкин on 18.04.2018.
//

#ifndef FS_FILESYSTEM_H
#define FS_FILESYSTEM_H
#include "inode.h"
#include "superblock.h"

typedef
struct FS_Handler
        FS_Handler;

extern void lock_inode_mutex(FS_Handler * fs_handler, struct inode* inode);
extern void unlock_inode_mutex(FS_Handler * fs_handler, struct inode* inode);
extern void shared_lock_inode_mutex(FS_Handler * fs_handler, struct inode* inode);
extern void shared_unlock_inode_mutex(FS_Handler * fs_handler, struct inode* inode);
extern short try_lock_inode_mutex(FS_Handler * fs_handler, struct inode* inode);


void * get_memory_for_filesystem(int number_of_inodes, int number_of_blocks);

struct inode * create_filesystem(char* filesystem, FS_Handler *fs_handler, int number_of_inodes, int number_of_blocks);
struct inode * open_filesystem(char* file_system_name, char* filesystem, FS_Handler *fs_handler, int number_of_inodes, int number_of_blocks);
char* save_filesystem(char* file_system_name, char* filesystem);
char* close_filesystem(char* file_system_name, char* filesystem);

char* ls(struct superblock *sb, struct inode* directory, FS_Handler *fs_handler);
char* mkdirf(struct superblock *sb, const char* name, struct inode* directory, FS_Handler *fs_handler);
char* rm_dir(struct superblock *sb, const char* name, struct inode* directory, FS_Handler *fs_handler, int id);
char* rm(struct superblock *sb, const char* name, struct inode* directory, FS_Handler *fs_handler, int id);
char* touch(struct superblock *sb, const char* name, const char* input, struct inode* directory, FS_Handler *fs_handler, int id);
char* read_file(struct superblock *sb, const char* name, struct inode* directory, short * failed, FS_Handler * fs_handler, int id);
char* cd(struct superblock *sb, const char*name, struct inode** current_directory, FS_Handler *fs_handler);

#endif //FS_FILESYSTEM_H
