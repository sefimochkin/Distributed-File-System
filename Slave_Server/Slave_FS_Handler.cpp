//
// Created by Сергей Ефимочкин on 2019-02-24.
//

#include "Slave_FS_Handler.h"

Slave_FS_Handler::Slave_FS_Handler(int number_of_blocks_, int number_of_free_blocks_){
    number_of_blocks = number_of_blocks_;

    filesystem = get_memory_for_filesystem(number_of_blocks);

    open_filesystem(name, (char*)filesystem, number_of_blocks);

    sb = (struct superblock *) filesystem;
}

unsigned int Slave_FS_Handler::store_data_blocks(std::string data){
    char * c_data =  const_cast<char*>(data.c_str());
    return put_data_in_blocks(sb, c_data, static_cast<int>(data.length()));
}

std::string Slave_FS_Handler::read_data_blocks(unsigned int index_of_blocks, int size_of_data){
    char* output = read_data_in_blocks(sb, &sb->blocks_array[index_of_blocks], size_of_data);
    std::string answer(output);
    free(output);
    return answer;
}
void Slave_FS_Handler::free_data_blocks(unsigned int index_of_blocks, int size_of_data){
    free_data_in_blocks(sb, &sb->blocks_array[index_of_blocks], size_of_data);
}

int Slave_FS_Handler::get_size_of_data_in_FS_blocks(int size_of_data){
    return get_size_of_data_in_blocks(sb, size_of_data);
}