//
// Created by Сергей Ефимочкин on 2019-02-24.
//

#ifndef DFS_SLAVE_SLAVES_FS_HANDLER_H
#define DFS_SLAVE_SLAVES_FS_HANDLER_H

extern "C" {
    #include "../slave_fs/FileSystem.h"
}
#include <string>
#include <unordered_map>

class Slave_FS_Handler {
public:

    Slave_FS_Handler(int number_of_blocks_, int number_of_free_blocks_);

    unsigned int store_data_blocks(std::string data);
    std::string read_data_blocks(unsigned int index_of_blocks, int size_of_data);
    void free_data_blocks(unsigned int index_of_blocks, int size_of_data);

    int get_size_of_data_in_FS_blocks(int size_of_data);

private:
    char * name = const_cast<char *>("./slave_fs_files");
    void* filesystem;
    struct superblock* sb;
    int number_of_blocks;

};


#endif //DFS_SLAVE_SLAVES_FS_HANDLER_H
