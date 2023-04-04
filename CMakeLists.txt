cmake_minimum_required(VERSION 3.16)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
project(msladkov-llp-lab3)
add_compile_options(-g)
set(CMAKE_VERBOSE_MAKEFILE OFF)

add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(parser)
# add_subdirectory(dto)