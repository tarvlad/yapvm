#include "interpreter.h"


#include <gtest/gtest.h>

#include "parser.h"
#include "utils.h"

using namespace yapvm;

TEST(interpreter_test, arithmetic) {
    testing::internal::CaptureStdout();
    interpreter::ThreadManager tm;
    scoped_ptr<Module> module = parser::generate_ast(trim(read_file_ast("test_resources/arithmetic.py")));
    interpreter::Interpreter interpreter(std::move(module), &tm);
    interpreter.launch();

    while (!tm.get_all_interpreters().empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    tm.finish_waiting();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "96 138 -729");
}


TEST(interpreter_test, fib_test) {
    testing::internal::CaptureStdout();
    interpreter::ThreadManager tm;
    scoped_ptr<Module> module = parser::generate_ast(trim(read_file_ast("test_resources/fib.py")));
    interpreter::Interpreter interpreter(std::move(module), &tm);
    interpreter.launch();

    while (!tm.get_all_interpreters().empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    tm.finish_waiting();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "4181");
}


TEST(interpreter_test, multithreaded_sum_test) {
    testing::internal::CaptureStdout();
    interpreter::ThreadManager tm;
    scoped_ptr<Module> module = parser::generate_ast(trim(read_file_ast("test_resources/mtsum.py")));
    interpreter::Interpreter interpreter(std::move(module), &tm);
    interpreter.launch();

    while (!tm.get_all_interpreters().empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    tm.finish_waiting();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "");
}