//
// Created by Сергей Ефимочкин on 2019-02-24.
//

#include "Slave_FS_Handler.h"

unsigned int Slave_FS_Handler::store_data_blocks(std::string data){
    char * c_data =  const_cast<char*>(data.c_str());
    return put_data_in_blocks(sb, c_data, static_cast<int>(strlen(data.c_str())));
}

std::string Slave_FS_Handler::read_data_blocks(unsigned int index_of_blocks, int size_of_data){
    char* output = read_data_in_blocks(sb, &sb->blocks_array[index_of_blocks], size_of_data);
    std::string answer(output);
    free(output);
    return answer;
}
void Slave_FS_Handler::free_data_blocks(unsigned int index_of_blocks, int size_of_data){
    free_data_in_blocks(sb, &sb->blocks_array[index_of_blocks], size_of_data, 0);
}