cmake_minimum_required(VERSION 3.16)

project(msladkov-llp-lab3-server)
add_subdirectory(db)

file(GLOB_RECURSE EXECUTOR_CXX_FILES ${CMAKE_CURRENT_SOURCE_DIR}/executor/*.cpp)
file(GLOB_RECURSE EXECUTOR_CXX_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/executor/*.hpp)

add_executable(${PROJECT_NAME} main.cpp ${EXECUTOR_CXX_FILES} ${EXECUTOR_CXX_HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE db)
target_link_libraries(${PROJECT_NAME} PRIVATE parser)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} PRIVATE dto)
