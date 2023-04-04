cmake_minimum_required(VERSION 3.16)

project(msladkov-llp-lab3-server)
add_subdirectory(db)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE db)
target_link_libraries(${PROJECT_NAME} PRIVATE parser)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
