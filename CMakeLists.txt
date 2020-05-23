cmake_minimum_required(VERSION 3.16)
project(C_Client_Server C)

set(CMAKE_C_STANDARD 99)

#set(INPUT_FILE_NAME input_file_little.txt)
set(INPUT_FILE_NAME input_file.txt)
configure_file(${INPUT_FILE_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${INPUT_FILE_NAME} COPYONLY)

add_executable(server server.c)
add_executable(client client.c)

add_executable(fread_exp fread_exp.c)
add_executable(array_exp array_exp.c)

#add_executable(daemon_exp daemon_exp.c)
