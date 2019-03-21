//
// Created by Сергей Ефимочкин on 18.04.2018.
//

#ifndef FS_SUPERBLOCK_H
#define FS_SUPERBLOCK_H

#include "block.h"

struct superblock {
    int number_of_blocks;
    int number_of_bytes_in_block;
    int number_of_chars_in_index;
    int number_of_free_blocks;

    void* blocks_bitmap;

    struct block* blocks_array;
    char* blocks_data_array;
};

struct block* get_free_block(struct superblock *sb);
struct block* get_n_continuous_free_blocks(struct superblock *sb, int number_of_blocks);
void free_block(struct superblock *sb, struct block* block);

void put_index_in_address_block(struct superblock *sb, char* address_block, unsigned int index);
struct block* get_block_by_index_in_address_block(struct superblock *sb, char* address_block);

#endif //FS_SUPERBLOCK_H
