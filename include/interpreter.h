#pragma once

#include <atomic>
#include <stack>
#include <thread>
#include "ast.h"

#include "scope.h"
#include "thread_manager.h"

//TODO add xmx xms

namespace yapvm::interpreter {
using namespace yapvm::ast;

//TODO stack of executing now loops (and functions probably for stacktrace???)
class Interpreter {
    Module *code_;
    Scope *scope_; // it is a current working scope
    Scope *main_scope_; // main scope for current interpreter, there can be other scopes
    std::atomic_bool parked_ = false;
    std::atomic_bool need_unpark_ = false;
    std::atomic_bool need_park_ = false;

    std::atomic_bool finishing_ = false;
    std::atomic_bool finished_ = false; // todo not atomic?
    std::thread worker_;

    ThreadManager *thread_manager_;

    std::stack<ManagedObject *> register_queue_;

    void __worker_exec(Module *code);

    void interpret_expr(Expr *code);

    bool interpret_stmt(Stmt *code);

    bool interpret(Node *code);

    void handle_safepoint();

public:
    Interpreter(scoped_ptr<Module> &&code, ThreadManager *tm, Scope *scope = new Scope{});
    ~Interpreter();

    void park();
    bool is_parked() const;
    void run();

    void launch();

    bool is_finished() const;
    void join();

    Scope *get_scope() const;

    std::vector<yobjects::ManagedObject *> get_register_queue();
};


}
