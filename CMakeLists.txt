cmake_minimum_required(VERSION 3.20)

project(yapvm)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
             
include_directories(include)


set(SOURCE_ALL
    include/parser.h
    src/parser.cpp

    include/lexer.h
    src/lexer.cpp

    include/utils.h
    src/utils.cpp
)


add_executable(yapvm 
    src/main.cpp
    ${SOURCE_ALL}
)


add_executable(yapvm_lexer_test
    test/lexer_test.cpp
    ${SOURCE_ALL}
)

enable_testing()

add_test(
    NAME yapvm_lexer_test
    COMMAND yapvm_lexer_test 
)

