cmake_minimum_required(VERSION 3.16)

project(server)
add_subdirectory(db)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} db)