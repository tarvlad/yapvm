#include <iostream>

#include "gc.h"
#include "interpreter.h"
#include "logger.h"
#include "parser.h"
#include "utils.h"

using namespace yapvm;
using namespace yapvm::ast;
using namespace yapvm::parser;
using namespace yapvm::interpreter;


int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Error: need to specify main file" << std::endl;
    } else if (argc > 4) {
        std::cout << "Error: too much args" << std::endl;
    }


    Logger::init_logger();

    scoped_ptr<Module> module = generate_ast(trim(read_file_ast(argv[1])));

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    ThreadManager tm;
    Interpreter *interpreter = new Interpreter(std::move(module), &tm);

    ygc::YGC* gc;
    if (argc == 4 && std::string(argv[2]) == "-Xmx") {
        std::string str_hs(argv[3]);
        ssize_t hs = std::stoi(str_hs);

        if (str_hs.ends_with("mb")) {
            hs *= 1024;
        } else if (str_hs.ends_with("gb")) {
            hs *= 1024 * 1024;
        }

        gc = new ygc::YGC(interpreter->get_scope(), &tm, hs);
    } else {
        gc = new ygc::YGC(interpreter->get_scope(), &tm);
    }

    interpreter->launch();

    gc->collect();

    //Logger::log("no-gc wait other threads");
    //while (!tm.get_all_interpreters().value().empty()) {
    //    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //}
    //tm.finish_waiting();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    Logger::log(
        "main process finished in "+
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()) +
        " [ms]"
    );

    delete(gc);

    return 0;
}
