cmake_minimum_required(VERSION 3.16)

project(msladkov-llp-lab3-client)

file(GLOB_RECURSE CXX_FILES src/*.cpp)
file(GLOB_RECURSE CXX_HEADERS src/*.hpp)

add_executable(${PROJECT_NAME} ${CXX_FILES} ${CXX_HEADERS})
target_link_libraries(${PROJECT_NAME} PUBLIC parser)