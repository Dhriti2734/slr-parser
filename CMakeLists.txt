cmake_minimum_required(VERSION 3.15)
project(SLRParser)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_executable(slr
    src/main.cpp
    src/lexer/Lexer.cpp
)