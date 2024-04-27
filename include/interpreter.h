#pragma once

#include <atomic>
#include <thread>
#include "ast.h"

#include "scope.h"

namespace yapvm::interpreter {
using namespace yapvm::ast;


class Interpreter {
    Module *code_;
    Scope *scope_; // it is a current working scope
    Scope *main_scope_; // main scope for current interpreter, there can be other scopes
    std::atomic_bool stopping_; // can be set to true only from caller thread, to false only from callee
    std::atomic_bool parked_; // can be modified only from callee thread
    std::atomic_bool resuming_; // can be set to true only from caller thread, to false only from callee

    std::atomic_bool finishing_;
    std::atomic_bool finished_; // todo not atomic?
    std::thread worker_;

    void __worker_exec(Module *code);

    void interpret_expr(Expr *code);

    void interpret_stmt(Stmt *code);

    void interpret(Node *code);
public:
    Interpreter(scoped_ptr<Module> &&code);
    ~Interpreter();

    void park();
    bool is_parked() const;
    void run();

    bool is_finished() const;
    void join();

    std::vector<yobjects::ManagedObject *> get_register_queue();
};


}
