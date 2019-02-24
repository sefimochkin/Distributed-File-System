cmake_minimum_required(VERSION 3.6)
project(DFS)
project(DFS_slave)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(Boost_USE_STATIC_LIBS        OFF) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
find_package(Boost 1.68.0 COMPONENTS thread filesystem system date_time REQUIRED)
if(Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
    #include_directories(${Boost_INCLUDE_DIRS})


    #target_link_libraries(DFS ${Boost_FILESYSTEM_LIBRARY} ${Boost_LIBRARIES})
endif(Boost_FOUND)

SET(USED_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY})

add_executable(DFS main.cpp fs/superblock.h fs/inode.h fs/FileSystem.h fs/FileSystem.c fs/superblock.c fs/inode.c fs/block.h fs/block.c
        utils/Network_utils.h utils/Network_utils.c utils/lock_free_queue.h FS_Server.h FS_Server.c Master_Server/Master_Server.cpp Master_Server/Master_Server.h utils/Server_Message.cpp utils/Server_Message.h Base_Classes/Inter_Server_Session.cpp Base_Classes/Inter_Server_Session.h Master_Server/Slaves_Group.cpp Master_Server/Slaves_Group.h Client_Server/Client_Server.cpp Client_Server/Client_Server.h Base_Classes/Server.h Base_Classes/Server_Group.cpp Base_Classes/Server_Group.h Master_Server/Initializing_ISS.h Master_Server/Clients_Group.cpp Master_Server/Clients_Group.h Master_Server/FS_Handler.cpp Master_Server/FS_Handler.h)

add_executable(DFS_slave Slave_Server/main_slave_server.cpp Slave_Server/Slave_Server.cpp Slave_Server/Slave_Server.h Slave_Server/main_slave_server.cpp utils/Server_Message.h utils/Server_Message.cpp Base_Classes/Server.h Base_Classes/Server_Group.cpp Base_Classes/Server_Group.h)

add_executable(DFS_client Client_Server/main_client_server.cpp Client_Server/Client_Server.cpp Client_Server/Client_Server.h Client_Server/main_client_server.cpp utils/Server_Message.h utils/Server_Message.cpp Base_Classes/Server.h Base_Classes/Server_Group.cpp Base_Classes/Server_Group.h)

TARGET_LINK_LIBRARIES(DFS ${USED_LIBS})
TARGET_LINK_LIBRARIES(DFS_slave ${USED_LIBS})
TARGET_LINK_LIBRARIES(DFS_client ${USED_LIBS})




