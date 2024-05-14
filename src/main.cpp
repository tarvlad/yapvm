#include <iostream>

#include "interpreter.h"
#include "parser.h"
#include "utils.h"

using namespace yapvm;
using namespace yapvm::ast;
using namespace yapvm::parser;
using namespace yapvm::interpreter;


int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Error: need to specify main file" << std::endl;
    }

    scoped_ptr<Module> module = generate_ast(trim(read_file_ast(argv[1])));

    ThreadManager tm;
    Interpreter interpreter(std::move(module), &tm);
    interpreter.launch();

    //TODO run gc instead of raw wait
    while (!tm.get_all_interpreters().empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    tm.finish_waiting();
    return 0;
}
