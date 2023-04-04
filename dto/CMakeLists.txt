cmake_minimum_required(VERSION 3.16)

project(dto)

add_library(${PROJECT_NAME} lib/dto.cpp lib/dto.hpp)
target_link_libraries(${PROJECT_NAME} PUBLIC parser)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
