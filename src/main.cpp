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
    constexpr bool gc_enabled = true;
    if (argc != 2) {
        std::cout << "Error: need to specify main file" << std::endl;
    }
    Logger::init_logger();

    scoped_ptr<Module> module = generate_ast(trim(read_file_ast(argv[1])));

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    ThreadManager tm;
    Interpreter interpreter(std::move(module), &tm);
    ygc::YGC gc(interpreter.get_scope(), &tm);
    interpreter.launch();

    if (gc_enabled) {
        gc.collect();
    } else {
        Logger::log("no-gc wait other threads");
        while (true) {
            std::optional<std::vector<Interpreter *>> interpreters_opt;
            do {
                interpreters_opt = tm.get_all_interpreters();
            } while (!interpreters_opt.has_value());

            if (!interpreters_opt.value().empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            } else {
                break;
            }
        }
        tm.finish_waiting();
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    Logger::log(
        "main process finished in "+
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()) +
        " [ms]"
    );
    return 0;
}
