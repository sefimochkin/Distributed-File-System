//
// Created by Сергей Ефимочкин on 18.04.2018.
//

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "FileSystem.h"
#include "../utils/Network_utils.h"


#define NUMBER_OF_INODES  1024
#define NUMBER_OF_BLOCKS  16384
#define NUMBER_OF_BYTES_IN_BLOCK  32
#define NUMBER_OF_CHARS_IN_INDEX  4
#define MAGIC_SYMBOLS "FILESYSTEM BY SERGEY EFIMOCHKIN"

void * get_memory_for_filesystem(){
    size_t size_of_filesystem = sizeof(struct superblock) + NUMBER_OF_INODES / 8 + NUMBER_OF_BLOCKS / 8
                                + NUMBER_OF_INODES * sizeof(struct inode) + NUMBER_OF_BLOCKS * sizeof(struct block) +
                                NUMBER_OF_BLOCKS*NUMBER_OF_BYTES_IN_BLOCK;
    void *filesystem = malloc(size_of_filesystem);
    memset(filesystem, 0, size_of_filesystem);

    return filesystem;
}


struct inode * create_filesystem(char *filesystem){

    struct superblock* sb = (struct superblock*) filesystem;
    sb->number_of_inods = NUMBER_OF_INODES;
    sb->number_of_blocks = NUMBER_OF_BLOCKS;
    sb->number_of_bytes_in_block = NUMBER_OF_BYTES_IN_BLOCK;
    sb->number_of_chars_in_index = NUMBER_OF_CHARS_IN_INDEX;
    sb->number_of_free_blocks = NUMBER_OF_BLOCKS;
    sb->number_of_free_inods = NUMBER_OF_INODES;
    sb->inods_bitmap =  (filesystem + sizeof(struct superblock));
    sb->blocks_bitmap = (filesystem + sizeof(struct superblock) + NUMBER_OF_INODES / 8);
    sb->inods_array = (struct inode*)(filesystem + sizeof(struct superblock) + NUMBER_OF_INODES / 8 + NUMBER_OF_BLOCKS / 8);
    sb->blocks_array = (struct block*)(filesystem + sizeof(struct superblock) + NUMBER_OF_INODES / 8 + NUMBER_OF_BLOCKS / 8 +
                                               NUMBER_OF_INODES * sizeof(struct inode));
    sb->blocks_data_array = (filesystem + sizeof(struct superblock) + NUMBER_OF_INODES / 8 + NUMBER_OF_BLOCKS / 8 +
                                                   NUMBER_OF_INODES * sizeof(struct inode) + NUMBER_OF_BLOCKS * sizeof(struct block));

    for (unsigned int i = 0; i < NUMBER_OF_INODES / 8; i++){
        ((unsigned int *)sb->inods_bitmap)[i] = 0;
    }

    for (unsigned int i = 0; i < NUMBER_OF_BLOCKS / 8; i++){
        ((unsigned int *)sb->blocks_bitmap)[i] = 0;
    }

    for(unsigned int i = 0; i < NUMBER_OF_INODES; i++){
        struct inode* inode = &sb->inods_array[i];
        inode->number_of_inode = i;
    }

    for(unsigned int i = 0; i < NUMBER_OF_BLOCKS; i++){
        struct block* block = &sb->blocks_array[i];
        block->number_of_block = i;
        sb->blocks_array[i].data = sb->blocks_data_array + i * NUMBER_OF_BYTES_IN_BLOCK;
    }

    struct inode * root =  create_directory(sb, "root", 4, NULL);

    return root;
}

struct inode * open_filesystem(char* file_system_name, char* filesystem, int sock){
    FILE* file_with_filesystem = fopen(file_system_name, "r");
    char* magic_symbols;

    if(file_with_filesystem == NULL)
    {
        struct inode* root = create_filesystem(filesystem);
        return root;
    }
    else{
        magic_symbols = malloc(strlen(MAGIC_SYMBOLS) + 1);
        memset(magic_symbols, 0, (strlen(MAGIC_SYMBOLS) + 1));
        fread(magic_symbols, sizeof(char), strlen(MAGIC_SYMBOLS), file_with_filesystem);
        if(strcmp(magic_symbols, MAGIC_SYMBOLS) != 0){
            send_answer(sock, "\nNot compatible file_system!");
            free(magic_symbols);
            exit(1);
        }
        free(magic_symbols);
    }

    struct superblock* sb = (struct superblock*) filesystem;
    fread(sb, sizeof(struct superblock), 1, file_with_filesystem);

    //restoring links
    sb->inods_bitmap =  (filesystem + sizeof(struct superblock));
    sb->blocks_bitmap = (filesystem + sizeof(struct superblock) + NUMBER_OF_INODES / 8);
    sb->inods_array = (struct inode*)(filesystem + sizeof(struct superblock) + NUMBER_OF_INODES / 8 + NUMBER_OF_BLOCKS / 8);
    sb->blocks_array = (struct block*)(filesystem + sizeof(struct superblock) + NUMBER_OF_INODES / 8 + NUMBER_OF_BLOCKS / 8 +
                                       NUMBER_OF_INODES * sizeof(struct inode));
    sb->blocks_data_array = (filesystem + sizeof(struct superblock) + NUMBER_OF_INODES / 8 + NUMBER_OF_BLOCKS / 8 +
                             NUMBER_OF_INODES * sizeof(struct inode) + NUMBER_OF_BLOCKS * sizeof(struct block));


    fread(sb->inods_bitmap, sizeof(char), NUMBER_OF_INODES / 8, file_with_filesystem);
    fread(sb->blocks_bitmap, sizeof(char), NUMBER_OF_BLOCKS / 8, file_with_filesystem);
    fread(sb->inods_array, sizeof(struct inode), NUMBER_OF_INODES, file_with_filesystem);
    fread(sb->blocks_array, sizeof(struct block), NUMBER_OF_BLOCKS, file_with_filesystem);
    fread(sb->blocks_data_array, sizeof(char), NUMBER_OF_BLOCKS * NUMBER_OF_BYTES_IN_BLOCK, file_with_filesystem);

    //restoring links
    for(unsigned int j = 0; j < NUMBER_OF_BLOCKS; j++){
        sb->blocks_array[j].data = sb->blocks_data_array + j * NUMBER_OF_BYTES_IN_BLOCK;
    }


    struct inode* root = sb->inods_array;

    fclose(file_with_filesystem);

    return root;
}

void save_filesystem(char* file_system_name, char* filesystem, int sock){

    FILE *file_with_filesystem = fopen(file_system_name, "w");

    if (file_with_filesystem == NULL)
    {
        send_answer(sock, "\nError saving to file!");
        exit (1);
    }

    fwrite(MAGIC_SYMBOLS, sizeof(char), strlen(MAGIC_SYMBOLS), file_with_filesystem);
    struct superblock* sb = (struct superblock *) filesystem;
    fwrite(sb, sizeof(struct superblock), 1 , file_with_filesystem);
    fwrite(sb->inods_bitmap, sizeof(char), NUMBER_OF_INODES / 8, file_with_filesystem);
    fwrite(sb->blocks_bitmap, sizeof(char), NUMBER_OF_BLOCKS / 8, file_with_filesystem);
    fwrite(sb->inods_array, sizeof(struct inode), NUMBER_OF_INODES, file_with_filesystem);
    fwrite(sb->blocks_array, sizeof(struct block), NUMBER_OF_BLOCKS, file_with_filesystem);
    fwrite(sb->blocks_data_array, sizeof(char), NUMBER_OF_BLOCKS * NUMBER_OF_BYTES_IN_BLOCK, file_with_filesystem);

    fclose(file_with_filesystem);
}

void close_filesystem(char* file_system_name, char* filesystem, int sock){
    save_filesystem(file_system_name, filesystem, sock);
    free(filesystem);
}

char* ls(struct superblock *sb, struct inode* directory){
    return get_file_names_from_directory(sb, directory);
}

void mkdir(struct superblock *sb, char* name, struct inode* directory, int sock){
    if(!check_doubling_name(sb, name, directory))
        send_answer(sock, "File with this name already exists!");
    else{
        if(sb->number_of_free_inods < 0)
            send_answer(sock, "No more free inods! Remove something!");

        else if(sb->number_of_free_blocks < get_size_of_data_in_blocks(sb, (int)strlen(name)))
            send_answer(sock, "No more free blocks! Remove something!");

        else
            create_directory(sb, name, (int)strlen(name), directory);
    }
}

void rm_dir(struct superblock *sb, char* name, struct inode* directory, int sock){
    struct inode* inode = get_inode_by_name(sb, name, directory, sock);
    if(inode != NULL)
        if(inode->is_directory)
            delete_directory(sb, inode);
        else
            send_answer(sock, "For deleting a file use rm");
}

void rm(struct superblock *sb, char* name, struct inode* directory, int sock){
    struct inode *inode = get_inode_by_name(sb, name, directory, sock);
    if(inode != NULL)
        if(inode->is_directory)
            send_answer(sock, "For deleting a directory use rmdir");
        else
            delete_file(sb, inode);
}

void touch(struct superblock *sb, char* name, char* input, struct inode* directory, int sock){
    if(!check_doubling_name(sb, name, directory))
        send_answer(sock, "File with this name already exists!");

    else{
        if(sb->number_of_free_inods < 0)
            send_answer(sock, "No more free inods! Remove something!");

        else if(sb->number_of_free_blocks < get_size_of_data_in_blocks(sb, (int)strlen(name)) +
                                            get_size_of_data_in_blocks(sb, (int)strlen(input)))
            send_answer(sock, "No more free blocks! Remove something!");

        else
            create_file(sb, name, input, (int)strlen(name), (int)strlen(input), directory);
    }
}

char* read_file(struct superblock *sb, char* name, struct inode* directory, int sock){
    struct inode *inode = get_inode_by_name(sb, name, directory, sock);

    if(inode != NULL) {
        if(inode->is_directory){
            send_answer(sock, "Can't read directory!");
            return NULL;
        }
        return open_file(sb, inode);
    }
    else return NULL;
}

void cd(struct superblock *sb, char*name, struct inode** current_directory, int sock){
    if(strcmp(name, "..") == 0)
        *current_directory = &(sb->inods_array[(*current_directory)->index_of_owner_inode]);
    else if (strcmp(name, "/") == 0)
        *current_directory = sb->inods_array; // root inode
    else {
        struct inode *inode = get_inode_by_name(sb, name, *current_directory, sock);
        if(inode != NULL) {
            if (!inode->is_directory)
                send_answer(sock, "Not a directory!");
            else
                *current_directory = inode;
        }
    }
}

void import_file(struct superblock *sb, char* inner_name, char* outer_name, struct inode* directory, int sock){
    char* file_path = malloc(strlen(outer_name) + 3);
    strncpy(file_path, "./", 2);
    strncpy(&file_path[2], outer_name, strlen(outer_name) + 1);

    FILE *filep = fopen(file_path, "r");

    if (filep == NULL)
    {
        send_answer(sock, "\nError opening file!");
        exit (1);
    }
    fseeko(filep, 0, SEEK_END);
    int size_of_file = (int) ftell(filep);
    rewind(filep);


    char* input = malloc(size_of_file + 1);
    memset(input, 0, (size_of_file +1));
    fread(input, sizeof(char), (size_t) size_of_file, filep);

    touch(sb, inner_name, input, directory, sock);

    fclose(filep);
    free(file_path);
    free(input);
}

void export_file(struct superblock *sb, char* inner_name, char* outer_name, struct inode* directory, int sock){
    char* file_path = malloc(strlen(outer_name) + 3);
    memset(file_path, 0, strlen(outer_name)+3);
    strncpy(file_path, "./", 2);
    strncpy(&file_path[2], outer_name, strlen(outer_name) + 1);

    FILE *filep = fopen(file_path, "w");

    if (filep == NULL)
    {
        send_answer(sock, "\nError opening file!");
        exit (1);
    }

    char* output = read_file(sb, inner_name, directory, sock);
    fwrite(output, sizeof(char), strlen(output), filep);
    fclose(filep);

    free(output);
    free(file_path);
}