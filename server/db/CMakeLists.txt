cmake_minimum_required(VERSION 3.16)

project(db LANGUAGES C)

file(GLOB_RECURSE C_FILES_LIB CONFIGURE_DEPENDS src/lib/*.c)
file(GLOB_RECURSE H_FILES_LIB CONFIGURE_DEPENDS src/*.h)

file(GLOB_RECURSE C_FILES_TESTS CONFIGURE_DEPENDS src/test/*.c)
file(GLOB_RECURSE H_FILES_TESTS CONFIGURE_DEPENDS src/test/*.h)

file(GLOB_RECURSE C_FILES_MAIN CONFIGURE_DEPENDS src/main/*.c src/benchmark/*.c)
file(GLOB_RECURSE H_FILES_MAIN CONFIGURE_DEPENDS src/main/*.h src/benchmark/*.h)

add_compile_options(
        -g -Wall -Wextra
        "$<$<CONFIG:SANITIZED>:-fsanitize=address,undefined,leak>"
)
add_link_options("$<$<CONFIG:SANITIZED>:-fsanitize=address,undefined,leak>")

add_library(${PROJECT_NAME} STATIC ${C_FILES_LIB} ${H_FILES_LIB} src/lib/storage_work/find_element.h src/lib/storage_work/find_element.c)
target_include_directories(${PROJECT_NAME} PUBLIC src/lib)

add_executable(${PROJECT_NAME}-tests ${C_FILES_TESTS} ${H_FILES_TESTS})
target_include_directories(${PROJECT_NAME}-tests PUBLIC src/tests)
target_link_libraries(${PROJECT_NAME}-tests ${PROJECT_NAME})

add_executable(${PROJECT_NAME}-benchmark ${C_FILES_MAIN} ${H_FILES_MAIN})
target_include_directories(${PROJECT_NAME}-benchmark PUBLIC src/main)
target_include_directories(${PROJECT_NAME}-benchmark PUBLIC src/benchmark)
target_link_libraries(${PROJECT_NAME}-benchmark ${PROJECT_NAME})

add_custom_target(testenv
        COMMAND ${CMAKE_COMMAND} -E make_directory testenv)

add_custom_target(test
        DEPENDS testenv
        WORKING_DIRECTORY testenv
        COMMAND touch test.bin
        COMMAND rm test.bin
        COMMAND ${PROJECT_NAME}-tests
        DEPENDS ${PROJECT_NAME}-tests
        )

add_custom_target(clang-format
        COMMAND
        clang-format
        -style=file
        --verbose
        -i
        ${C_FILES_MAIN} ${H_FILES_MAIN} ${C_FILES_LIB} ${H_FILES_LIB} ${C_FILES_TEST} ${H_FILES_TEST}
        WORKING_DIRECTORY
        ${CMAKE_SOURCE_DIR}
        COMMENT
        "Running clang-format on sources..."
        )