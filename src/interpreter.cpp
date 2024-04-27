#include "interpreter.h"
#include <chrono>


// SHOULD be called only once when interpreter starts
void yapvm::interpreter::Interpreter::__worker_exec(Module *code) {
    while (!resuming_.load()) {
        std::this_thread::sleep_for(std::chrono::nanoseconds{100});
    }

    for (const scoped_ptr<Stmt> &i: code->body()) {
        interpret(i);
        if (finishing_.load()) {
            break;
        }
    }
    finishing_.store(true);
    resuming_.store(false);
    parked_.store(true);
    stopping_.store(false);
    finished_.store(true);
    finishing_.store(false);
}


void yapvm::interpreter::Interpreter::interpret_expr(Expr *code) {
    std::terminate(); //TODO after every expr exec change lst_expr_res ???
}


void yapvm::interpreter::Interpreter::interpret_stmt(Stmt *code) {
    assert(code != nullptr);

    if (instanceof<Import>(code)) {
        return; // currently just ignore
    }
    if (instanceof<FunctionDef>(code)) {
        FunctionDef *fdef = dynamic_cast<FunctionDef *>(code);
        scope_->add_function(fdef->name(), fdef);
        return;
    }
    if (instanceof<ClassDef>(code)) {
        throw std::runtime_error("Interpreter: ClassDef currently not supported");
    }
    if (instanceof<Return>(code)) {
        Return *rt = dynamic_cast<Return *>(code);

        if (scope_ == main_scope_) {
            finishing_.store(true);
            return;
        }
        if (rt->returns_anything()) {
            interpret_expr(rt->value());
        }
        Scope *prev = scope_;
        scope_ = scope_->parent();
        scope_->change(Scope::lst_exec_res, prev->get(Scope::lst_exec_res).value());
        delete prev;
        return;
    }
    if (instanceof<Assign>(code)) {
        Assign *assign = dynamic_cast<Assign *>(code);
        if (assign->target().size() != 1 || !instanceof<Name>(assign->target()[0].get())) {
            throw std::runtime_error("Interpreter: currently can assign only to single Name");
        }
        Name *target = dynamic_cast<Name *>(assign->target()[0].get());
        assert(target != nullptr);
        interpret_expr(assign->value());
        scope_->store_last_exec_res(target->id());
        return;
    }
    //TODO
}


void yapvm::interpreter::Interpreter::interpret(Node *code) {
    assert(code != nullptr);
    if (instanceof<Stmt>(code)) {
        interpret_stmt(dynamic_cast<Stmt *>(code));
        //TODO check and park???
        if (finishing_.load()) {
            return;
        }
    }
}


yapvm::interpreter::Interpreter::Interpreter(scoped_ptr<Module> &&code)
    : code_{code.steal()}, scope_{new Scope{}}, main_scope_{scope_}, worker_{__worker_exec, this, code_} {}


yapvm::interpreter::Interpreter::~Interpreter() { delete code_; }


void yapvm::interpreter::Interpreter::park() { stopping_.store(true); }


bool yapvm::interpreter::Interpreter::is_parked() const {
    return parked_.load();
}


void yapvm::interpreter::Interpreter::run() {
    while (stopping_.load()) {
    } // wait while thread parks
    resuming_.store(true); // all {stopping <- true} actions should hb {resuming <- true} actions
}

bool yapvm::interpreter::Interpreter::is_finished() const { return finished_.load(); }


void yapvm::interpreter::Interpreter::join() {
    worker_.join();
}


// TODO
std::vector<yapvm::yobjects::ManagedObject *> yapvm::interpreter::Interpreter::get_register_queue() {
    std::terminate();
    return {};
}
