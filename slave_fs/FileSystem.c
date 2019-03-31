//
// Created by Сергей Ефимочкин on 18.04.2018.
//

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "FileSystem.h"
#include "../utils/Network_utils.h"


#define NUMBER_OF_BYTES_IN_BLOCK  32
#define NUMBER_OF_CHARS_IN_INDEX  4
#define MAGIC_SYMBOLS "SLAVE OF DFS BY SERGEY EFIMOCHKIN"

void * get_memory_for_filesystem(int number_of_blocks){
    size_t size_of_filesystem = sizeof(struct superblock)  + number_of_blocks / 8
            + number_of_blocks * sizeof(struct block) +
            number_of_blocks * NUMBER_OF_BYTES_IN_BLOCK;
    void *filesystem = malloc(size_of_filesystem);
    memset(filesystem, 0, size_of_filesystem);

    return filesystem;
}


void create_filesystem(char *filesystem, int number_of_blocks){

    struct superblock* sb = (struct superblock*) filesystem;
    sb->number_of_blocks = number_of_blocks;
    sb->number_of_bytes_in_block = NUMBER_OF_BYTES_IN_BLOCK;
    sb->number_of_chars_in_index = NUMBER_OF_CHARS_IN_INDEX;
    sb->number_of_free_blocks = number_of_blocks;
    sb->blocks_bitmap = (filesystem + sizeof(struct superblock));
    sb->blocks_array = (struct block*)(filesystem + sizeof(struct superblock) + number_of_blocks / 8);
    sb->blocks_data_array = (filesystem + sizeof(struct superblock) + number_of_blocks / 8 + number_of_blocks * sizeof(struct block));

    for (unsigned int i = 0; i < number_of_blocks / 8; i++){
        ((unsigned int *)sb->blocks_bitmap)[i] = 0;
    }

    for(unsigned int i = 0; i < number_of_blocks; i++){
        struct block* block = &sb->blocks_array[i];
        block->number_of_block = i;
        sb->blocks_array[i].data = sb->blocks_data_array + i * NUMBER_OF_BYTES_IN_BLOCK;
    }

}

void open_filesystem(char* file_system_name, char* filesystem, int number_of_blocks){
    FILE* file_with_filesystem = fopen(file_system_name, "r");
    char* magic_symbols;

    if(file_with_filesystem == NULL)
    {
        create_filesystem(filesystem, number_of_blocks);
        return;
    }

    magic_symbols = malloc(strlen(MAGIC_SYMBOLS) + 1);
    memset(magic_symbols, 0, (strlen(MAGIC_SYMBOLS) + 1));
    fread(magic_symbols, sizeof(char), strlen(MAGIC_SYMBOLS), file_with_filesystem);
    free(magic_symbols);

    struct superblock* sb = (struct superblock*) filesystem;
    fread(sb, sizeof(struct superblock), 1, file_with_filesystem);

    //restoring links
    sb->blocks_bitmap = (filesystem + sizeof(struct superblock));
    sb->blocks_array = (struct block*)(filesystem + sizeof(struct superblock) + number_of_blocks / 8);
    sb->blocks_data_array = (filesystem + sizeof(struct superblock) + number_of_blocks / 8 + number_of_blocks * sizeof(struct block));


    fread(sb->blocks_bitmap, sizeof(char), number_of_blocks / 8, file_with_filesystem);
    fread(sb->blocks_array, sizeof(struct block), number_of_blocks, file_with_filesystem);
    fread(sb->blocks_data_array, sizeof(char), number_of_blocks * NUMBER_OF_BYTES_IN_BLOCK, file_with_filesystem);

    //restoring links
    for(unsigned int j = 0; j < number_of_blocks; j++){
        sb->blocks_array[j].data = sb->blocks_data_array + j * NUMBER_OF_BYTES_IN_BLOCK;
    }

    fclose(file_with_filesystem);
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
    fwrite(sb->blocks_bitmap, sizeof(char), sb->number_of_blocks / 8, file_with_filesystem);
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

int get_size_of_data_in_blocks(struct superblock *sb, int size_of_data){
    int size = 0;
    size += get_number_of_address_blocks(sb, size_of_data);

    int added_block = 0;
    if (size_of_data % sb->number_of_bytes_in_block > 0)
        added_block = 1;

    size += size_of_data / sb->number_of_bytes_in_block + added_block;

    return size;
}