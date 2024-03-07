cmake_minimum_required(VERSION 3.20)

project(yapvm)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
             
include_directories(include)


add_executable(yapvm 
    src/main.cpp
    
    include/parser.h
    src/parser.cpp

    include/lexer.h
    src/lexer.cpp

    include/utils.h
    src/utils.cpp
)