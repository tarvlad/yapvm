cmake_minimum_required(VERSION 3.20)

project(yapvm)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
include_directories(include)

set(SOURCE_ALL
    include/parser.h
    src/parser.cpp

    include/utils.h
    src/utils.cpp

    include/ast.h
    src/ast.cpp

    include/y_objects.h
    src/y_objects.cpp

    include/gc.h
    src/gc.cpp

    include/interpreter.h
    src/interpreter.cpp
)


add_executable(yapvm 
    src/main.cpp
    ${SOURCE_ALL}
)

add_executable(yapvm_parser_test
    test/parser_test.cpp
    ${SOURCE_ALL}
)

enable_testing()

add_test(
    NAME yapvm_lexer_test
    COMMAND yapvm_lexer_test 
)

add_test(
    NAME yapvm_parser_test
    COMMAND yapvm_parser_test
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(YObjectTest 
  test/YObjectTest.cpp
  ${SOURCE_ALL}
)
target_link_libraries(
  YObjectTest
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(YObjectTest)
