//
// Created by Сергей Ефимочкин on 18.04.2018.
//

#ifndef FS_INODE_H
#define FS_INODE_H

#include "superblock.h"
#include "block.h"
//#include "../Master_Server/FS_Handler.h"


struct inode {
    int size_of_name_in_chars;
    int size_of_file_in_chars;

    unsigned int index_of_blocks_array_of_name;
    unsigned int index_of_blocks_array_of_file;

    short is_directory;
    int number_of_files_in_directory;
    unsigned int index_of_blocks_array_of_inodes;

    unsigned int index_of_owner_inode;
    unsigned int number_of_inode;
};

typedef
struct FS_Handler
        FS_Handler;

extern void store_data_in_slave_wrapper(FS_Handler * fs_handler, int id,  int inode_id, char *data);
extern void read_data_in_slave_wrapper(FS_Handler * fs_handler, int id, int inode_id);
extern void free_data_in_slave_wrapper(FS_Handler * fs_handler, int id, int inode_id);

struct inode * create_file(struct superblock *sb, char* name, char* file, int size_of_name, int size_of_file, struct inode* owner, FS_Handler *fs_handler, int id);
struct inode * create_directory(struct superblock *sb, char* name, int size_of_name, struct inode* owner, FS_Handler *fs_handler, short is_root);
void add_file_to_directory(struct superblock *sb, struct inode* directory, struct inode* added_inode, FS_Handler *fs_handler);


void delete_file(struct superblock *sb, struct inode* inode, FS_Handler *fs_handler, int id, char* name);
void delete_directory(struct superblock *sb, struct inode* inode, FS_Handler *fs_handler, int id, char* name);
void delete_file_from_directory(struct superblock *sb, struct inode* deleted_inode, FS_Handler *fs_handler, int id, char* name);

char* get_file_name(struct superblock *sb, struct inode* inode);
void open_file(FS_Handler *fs_handler, int id, int inode_id);

char* get_file_names_from_directory(struct superblock *sb, struct inode* directory, FS_Handler *fs_handler);

struct inode* get_inode_by_name(struct superblock *sb, char*name, struct inode* directory, char** answer);
short check_doubling_name(struct superblock *sb, char*name, struct inode* directory);

int get_size_of_data_in_blocks(struct superblock *sb, int size_of_data);
int get_approximate_size_of_blocks(struct superblock *sb, int n_of_blocks);

#endif //FS_INODE_H
