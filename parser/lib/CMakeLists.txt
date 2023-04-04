cmake_minimum_required(VERSION 3.16)


find_package(FLEX)
find_package(BISON)

set(CMAKE_CONFIGURATION_TYPES Debug Sanitized Release)
set(PARSER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/parser)
set(PARSER_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated)
set(LEXER_CONFIG ${PARSER_SRC_DIR}/parser.l)
set(PARSER_GRAMMAR ${PARSER_SRC_DIR}/parser.y)

flex_target(lexer 
  ${LEXER_CONFIG}
  ${PARSER_OUT_DIR}/lexer.cpp DEFINES_FILE ${PARSER_OUT_DIR}/lexer.hpp
)

bison_target(parser
  ${PARSER_GRAMMAR}
  ${PARSER_OUT_DIR}/parser.cpp
  DEFINES_FILE ${PARSER_OUT_DIR}/parser.tab.hpp)

add_flex_bison_dependency(lexer parser)

add_library(${PROJECT_NAME} STATIC
            ast_printer.cpp
            parser_driver.cpp
            json_to_ast.cpp
            ${BISON_parser_OUTPUTS}
            ${FLEX_lexer_OUTPUTS}
)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(${PROJECT_NAME} PRIVATE ${PARSER_OUT_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
