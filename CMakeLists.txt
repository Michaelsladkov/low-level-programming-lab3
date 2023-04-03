cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
project(msladkov-llp-lab3)
set(CMAKE_VERBOSE_MAKEFILE OFF)

add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(parser)
# add_subdirectory(dto)