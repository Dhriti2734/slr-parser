cmake_minimum_required(VERSION 3.10)
project(SLRParser)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

add_executable(slr_parser
    src/main.cpp
    src/grammar/grammar.cpp
    src/parser/lr0.cpp
    src/parser/slr_table.cpp
    src/parser/parser.cpp
    src/parse_tree/parse_tree.cpp
    src/ast/ast.cpp
)
