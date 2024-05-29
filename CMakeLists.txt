cmake_minimum_required(VERSION 3.20)

project(yapvm)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-DMAX_HS=500)
#add_link_options(-fsanitize=address)

include_directories(include)
include_directories(test_resources)

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

        include/allocator.h
        src/allocator.cpp

        include/kvstorage.h
        include/kvstorage_element.h

        include/scope.h
        src/scope.cpp

        include/thread_manager.h
        src/thread_manager.cpp

        include/logger.h
        src/logger.cpp
)


add_executable(yapvm
        src/main.cpp
        ${SOURCE_ALL}
)


include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(y_object_test
        test/y_object_test.cpp
        ${SOURCE_ALL}
)

add_executable(ygc_test
        test/ygc_test.cpp
        ${SOURCE_ALL}
)

add_executable(parser_test
        test/parser_test.cpp
        ${SOURCE_ALL}
)


add_executable(kv_storage_test
        test/kv_storage_test.cpp
        ${SOURCE_ALL}
)

add_executable(scope_test
        test/scope_test.cpp
        ${SOURCE_ALL}
)

add_executable(interpreter_test
        test/interpreter_test.cpp
        ${SOURCE_ALL}
)

target_link_libraries(
        y_object_test
        GTest::gtest_main
)

target_link_libraries(
        ygc_test
        GTest::gtest_main
)

target_link_libraries(
        parser_test
        GTest::gtest_main
)

target_link_libraries(
        kv_storage_test
        GTest::gtest_main
)

target_link_libraries(
        scope_test
        GTest::gtest_main
)

target_link_libraries(
        interpreter_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(y_object_test)
gtest_discover_tests(ygc_test)
gtest_discover_tests(parser_test)
gtest_discover_tests(kv_storage_test)
gtest_discover_tests(interpreter_test)