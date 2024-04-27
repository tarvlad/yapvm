#pragma once

#include "ast.h"
#include <thread>
#include <atomic>

namespace yapvm::interpreter {
using namespace yapvm::ast;


class Interpreter {
    Module *code_;
    Node *ic_; // interpreter counter
    std::atomic_bool stopping_; // can be set to true only from caller thread, to false only from callee
    std::atomic_bool parked_; // can be modified only from callee thread
    std::atomic_bool resuming_; // can be set to true only from caller thread, to false only from callee


public:
    Interpreter(scoped_ptr<Module> &&code);
    ~Interpreter();

    void park();
    bool is_parked() const;
    void run();

    std::vector<yobjects::ManagedObject *> get_register_queue();
};


}
