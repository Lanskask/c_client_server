cmake_minimum_required(VERSION 3.16)
project(C_Client_Server C)

set(CMAKE_C_STANDARD 99)

configure_file(input_file.txt ${CMAKE_CURRENT_BINARY_DIR}/input_file.txt COPYONLY)


add_executable(server server.c)
add_executable(client client.c)

add_executable(fread_exp fread_exp.c)

#add_executable(server2 server2.c)
#add_executable(client2 client2.c)
#
#add_executable(for_tests for_tests.c)
#add_executable(for_tests_sig_handle for_tests_sig_handle.c)
#add_executable(daemon_exp daemon_exp.c)
