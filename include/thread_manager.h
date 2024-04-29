#pragma once
#include "interpreter.h"


namespace yapvm::interpreter {

// not thread-safe, there can be only one instance in gc
class ThreadManager {
    std::vector<Interpreter *> interpreters_;

public:
    void register_interpreter(Interpreter *interpreter);
    void unregister_interpreter(Interpreter *interpreter);

    std::vector<Interpreter *> get_all_interpreters() const;
    void park_all();
    bool is_all_parked() const;
    void run_all();
};

}