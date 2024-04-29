#pragma once
#include "interpreter.h"


namespace yapvm::interpreter {

class ThreadManager {


public:
    ThreadManager() = default;
    ~ThreadManager() =  default;

    void register_interpreter(Interpreter *interpreter);
    void unregister_interpreter();

    std::vector<Interpreter *> get_all_interpreters() const;
    void park_all();
    bool is_all_parked();
    void run_all();
};

}
