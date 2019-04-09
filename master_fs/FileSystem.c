//
// Created by Сергей Ефимочкин on 18.04.2018.
//

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "FileSystem.h"
#include "../utils/Network_utils.h"


//#define NUMBER_OF_INODES  1024
//#define NUMBER_OF_BLOCKS  16384
#define NUMBER_OF_BYTES_IN_BLOCK  32
#define NUMBER_OF_CHARS_IN_INDEX  4
#define MAGIC_SYMBOLS "MASTER OF DFS BY SERGEY EFIMOCHKIN"

void * get_memory_for_filesystem(int number_of_inodes, int number_of_blocks){
    size_t size_of_filesystem = sizeof(struct superblock) + number_of_inodes / 8 + number_of_blocks / 8
                                + number_of_inodes * sizeof(struct inode) + number_of_blocks * sizeof(struct block) +
            number_of_blocks * NUMBER_OF_BYTES_IN_BLOCK;
    void *filesystem = malloc(size_of_filesystem);
    memset(filesystem, 0, size_of_filesystem);

    return filesystem;
}


struct inode * create_filesystem(char *filesystem, struct FS_Handler * fs_handler, int number_of_inodes, int number_of_blocks){

    struct superblock* sb = (struct superblock*) filesystem;
    sb->number_of_inods = number_of_inodes;
    sb->number_of_blocks = number_of_blocks;
    sb->number_of_bytes_in_block = NUMBER_OF_BYTES_IN_BLOCK;
    sb->number_of_chars_in_index = NUMBER_OF_CHARS_IN_INDEX;
    sb->number_of_free_blocks = number_of_blocks;
    sb->number_of_free_inods = number_of_inodes;
    sb->inods_bitmap =  (filesystem + sizeof(struct superblock));
    sb->blocks_bitmap = (filesystem + sizeof(struct superblock) + number_of_inodes / 8);
    sb->inods_array = (struct inode*)(filesystem + sizeof(struct superblock) + number_of_inodes / 8 + number_of_blocks / 8);
    sb->blocks_array = (struct block*)(filesystem + sizeof(struct superblock) + number_of_inodes / 8 + number_of_blocks / 8 +
            number_of_inodes * sizeof(struct inode));
    sb->blocks_data_array = (filesystem + sizeof(struct superblock) + number_of_inodes / 8 + number_of_blocks / 8 +
            number_of_inodes * sizeof(struct inode) + number_of_blocks * sizeof(struct block));

    for (unsigned int i = 0; i < number_of_inodes / 8; i++){
        ((unsigned int *)sb->inods_bitmap)[i] = 0;
    }

    for (unsigned int i = 0; i < number_of_blocks / 8; i++){
        ((unsigned int *)sb->blocks_bitmap)[i] = 0;
    }

    for(unsigned int i = 0; i < number_of_inodes; i++){
        struct inode* inode = &sb->inods_array[i];
        inode->number_of_inode = i;
    }

    for(unsigned int i = 0; i < number_of_blocks; i++){
        struct block* block = &sb->blocks_array[i];
        block->number_of_block = i;
        sb->blocks_array[i].data = sb->blocks_data_array + i * NUMBER_OF_BYTES_IN_BLOCK;
    }

    struct inode * root =  create_directory(sb, "root", 4, NULL, fs_handler, 1);

    return root;
}

struct inode * open_filesystem(char* file_system_name, char* filesystem, FS_Handler *fs_handler, int number_of_inodes, int number_of_blocks){
    FILE* file_with_filesystem = fopen(file_system_name, "r");
    char* magic_symbols;

    if(file_with_filesystem == NULL)
    {
        struct inode* root = create_filesystem(filesystem, fs_handler, number_of_inodes, number_of_blocks);
        return root;
    }

    magic_symbols = malloc(strlen(MAGIC_SYMBOLS) + 1);
    memset(magic_symbols, 0, (strlen(MAGIC_SYMBOLS) + 1));
    fread(magic_symbols, sizeof(char), strlen(MAGIC_SYMBOLS), file_with_filesystem);
    free(magic_symbols);

    struct superblock* sb = (struct superblock*) filesystem;
    fread(sb, sizeof(struct superblock), 1, file_with_filesystem);

    //restoring links
    sb->inods_bitmap =  (filesystem + sizeof(struct superblock));
    sb->blocks_bitmap = (filesystem + sizeof(struct superblock) + number_of_inodes / 8);
    sb->inods_array = (struct inode*)(filesystem + sizeof(struct superblock) + number_of_inodes / 8 + number_of_blocks / 8);
    sb->blocks_array = (struct block*)(filesystem + sizeof(struct superblock) + number_of_inodes / 8 + number_of_blocks / 8 +
            number_of_inodes * sizeof(struct inode));
    sb->blocks_data_array = (filesystem + sizeof(struct superblock) + number_of_inodes / 8 + number_of_blocks / 8 +
            number_of_inodes * sizeof(struct inode) + number_of_blocks * sizeof(struct block));


    fread(sb->inods_bitmap, sizeof(char), number_of_inodes / 8, file_with_filesystem);
    fread(sb->blocks_bitmap, sizeof(char), number_of_blocks / 8, file_with_filesystem);
    fread(sb->inods_array, sizeof(struct inode), number_of_inodes, file_with_filesystem);
    fread(sb->blocks_array, sizeof(struct block), number_of_blocks, file_with_filesystem);
    fread(sb->blocks_data_array, sizeof(char), number_of_blocks * NUMBER_OF_BYTES_IN_BLOCK, file_with_filesystem);

    //restoring links
    for(unsigned int j = 0; j < number_of_blocks; j++){
        sb->blocks_array[j].data = sb->blocks_data_array + j * NUMBER_OF_BYTES_IN_BLOCK;
    }


    struct inode* root = sb->inods_array;

    fclose(file_with_filesystem);

    return root;
}

char* save_filesystem(char* file_system_name, char* filesystem){
    char* answer;

    FILE *file_with_filesystem = fopen(file_system_name, "w");

    if (file_with_filesystem == NULL)
    {
        answer = "\nError saving to file!";
        return answer;
    }

    fwrite(MAGIC_SYMBOLS, sizeof(char), strlen(MAGIC_SYMBOLS), file_with_filesystem);
    struct superblock* sb = (struct superblock *) filesystem;
    fwrite(sb, sizeof(struct superblock), 1 , file_with_filesystem);
    fwrite(sb->inods_bitmap, sizeof(char), sb->number_of_inods / 8, file_with_filesystem);
    fwrite(sb->blocks_bitmap, sizeof(char), sb->number_of_blocks / 8, file_with_filesystem);
    fwrite(sb->inods_array, sizeof(struct inode), sb->number_of_inods, file_with_filesystem);
    fwrite(sb->blocks_array, sizeof(struct block), sb->number_of_blocks, file_with_filesystem);
    fwrite(sb->blocks_data_array, sizeof(char), sb->number_of_blocks * NUMBER_OF_BYTES_IN_BLOCK, file_with_filesystem);

    fclose(file_with_filesystem);

    answer = "";
    return answer;
}

char* close_filesystem(char* file_system_name, char* filesystem){
    char* answer = save_filesystem(file_system_name, filesystem);
    free(filesystem);

    return answer;
}

short is_directory_with_files(struct inode** directory) {
    if ((*directory)->number_of_files_in_directory)
        return 1;
    else
        return 0;
}


char* ls(struct superblock *sb, struct inode** directory, FS_Handler *fs_handler){
    return get_file_names_from_directory(sb, *directory, fs_handler);
}

char* mkdirf(struct superblock *sb, const char* name, struct inode** directory, FS_Handler * fs_handler){
    char* answer;
    if(check_doubling_name(sb, name, *directory) == 0)
        answer = "File with this name already exists!";
    else{
        if(sb->number_of_free_inods < 0)
            answer = "No more free inods! Remove something!";

        else if(sb->number_of_free_blocks < get_size_of_data_in_blocks(sb, (int)strlen(name)))
            answer = "No more free blocks! Remove something!";

        else {
            create_directory(sb, name, (int) strlen(name), *directory, fs_handler, 0);
            answer = "";
        }
    }
    return answer;
}

char* rm_dir(struct superblock *sb, const char* name, struct inode** directory, FS_Handler *fs_handler, int id){
    char *answer = NULL;
    struct inode* inode = get_inode_by_name(sb, (char *) name, *directory, answer);
    if(inode != NULL)
        if(inode->is_directory) {
            if (try_lock_inode_mutex(fs_handler, inode)) {
                delete_directory(sb, inode, fs_handler, id, (char *) name);
                answer = "";
                unlock_inode_mutex(fs_handler, inode);
            }
            else
                answer = "Somebody else is inside the directory, can't delete!";
        }
        else {
            answer = "For deleting a file use rm";
        }

    return answer;
}

char* rm(struct superblock *sb, const char* name, struct inode** directory, FS_Handler *fs_handler, int id){
    char *answer = NULL;
    struct inode *inode = get_inode_by_name(sb, (char *) name, *directory, answer);
    if(inode != NULL)
        if(inode->is_directory)
            answer = "For deleting a directory use rmdir";
        else {
            lock_inode_mutex(fs_handler, inode);

            delete_file(sb, inode, fs_handler, id, (char *) name);
            answer = "";

            unlock_inode_mutex(fs_handler, inode);
        }
    return answer;
}

char* touch(struct superblock *sb, const char* name, const char* input, struct inode** directory, FS_Handler *fs_handler, int id){
    char* answer;
    if(!check_doubling_name(sb, name, *directory))
        answer = "File with this name already exists!";

    else{
        if(sb->number_of_free_inods < 0)
            answer = "No more free inods! Remove something!";

        else if(sb->number_of_free_blocks < get_size_of_data_in_blocks(sb, (int)strlen(name)) +
                                            get_size_of_data_in_blocks(sb, (int)strlen(input)))
            answer = "No more free blocks! Remove something!";

        else {
            create_file(sb, name, input, (int) strlen(name), (int) strlen(input), *directory, fs_handler, id);
            answer = "";
        }
    }
    return answer;
}


char* read_file(struct superblock *sb, const char* name, struct inode** directory, short int* failed, FS_Handler * fs_handler, int id){
    char* answer = NULL;
    if(directory == NULL) {
        *failed = 2;
        answer = "Something went wrong! Sorry, it's still buggy as hell";
        return answer;
    }

    struct inode *inode = get_inode_by_name(sb, (char *) name, *directory, &answer);

    if(inode != NULL) {
        if(inode->is_directory) {
            *failed = 1;
            answer = "Can't read directory!";
            return answer;
        }
        shared_lock_inode_mutex(fs_handler, inode);

        open_file(fs_handler, id, inode->number_of_inode);

        shared_unlock_inode_mutex(fs_handler, inode); // can it break because of asynchronous communication with slave?
                                                        // maybe it's better to unlock upon receiving the answer from slave?
        return "";
    }
    else {
        *failed = 1;
        return answer;
    }
}

char* cd(struct superblock *sb, const char* name, struct inode** current_directory, FS_Handler *fs_handler){
    char* answer = NULL;
    if(strcmp(name, "..") == 0) {
        shared_unlock_inode_mutex(fs_handler, *current_directory);
        *current_directory = &(sb->inods_array[(*current_directory)->index_of_owner_inode]);
        answer = "";
    }
    else if (strcmp(name, "/") == 0) {
        while (&(sb->inods_array[(*current_directory)->index_of_owner_inode]) != sb->inods_array){ //going up to the root
            shared_unlock_inode_mutex(fs_handler, *current_directory);
            *current_directory = &(sb->inods_array[(*current_directory)->index_of_owner_inode]);
        }
        //*current_directory = sb->inods_array; // root inode
        answer = "";
    }
    else {
        struct inode *inode = get_inode_by_name(sb, name, *current_directory, answer);
        if(inode != NULL) {
            if (!inode->is_directory)
                answer =  "Not a directory!";
            else {
                shared_lock_inode_mutex(fs_handler, inode);
                *current_directory = inode;
                answer = "";
            }
        }
    }

    return answer;
}