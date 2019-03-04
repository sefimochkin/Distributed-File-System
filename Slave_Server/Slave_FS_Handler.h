//
// Created by Сергей Ефимочкин on 2019-02-24.
//

#ifndef DFS_SLAVE_SLAVES_FS_HANDLER_H
#define DFS_SLAVE_SLAVES_FS_HANDLER_H

extern "C" {
    #include "../fs/FileSystem.h"
}
#include <string>
#include <unordered_map>

class Slave_FS_Handler {
public:

    Slave_FS_Handler(){

        filesystem = get_memory_for_filesystem();

        root = open_filesystem(name, (char*)filesystem);

        sb = (struct superblock *) filesystem;
    }

    unsigned int store_data_blocks(std::string data);
    std::string read_data_blocks(unsigned int index_of_blocks, int size_of_data);
    void free_data_blocks(unsigned int index_of_blocks, int size_of_data);

private:
    char * name = const_cast<char *>("./slave_fs_files");
    void* filesystem;
    struct inode* root;
    struct superblock* sb;

};


#endif //DFS_SLAVE_SLAVES_FS_HANDLER_H
