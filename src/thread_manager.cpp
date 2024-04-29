#include "thread_manager.h"
#include "interpreter.h"

void yapvm::interpreter::ThreadManager::register_interpreter(Interpreter *interpreter) {
    interpreters_.push_back(interpreter);
}


void yapvm::interpreter::ThreadManager::unregister_interpreter(Interpreter *interpreter) {
    for (size_t i = 0; i < interpreters_.size(); i++) {
        if (interpreters_[i] == interpreter) {
            std::vector<Interpreter *>::iterator it = interpreters_.begin();
            std::advance(it, i);
            interpreters_.erase(it);
            break;
        }
    }
    join_queue_.push_back(interpreter);
}


std::vector<yapvm::interpreter::Interpreter *> yapvm::interpreter::ThreadManager::get_all_interpreters() const {
    return interpreters_;
}


void yapvm::interpreter::ThreadManager::park_all() {
    for (Interpreter *interpreter : interpreters_) {
        interpreter->park();
        while (!interpreter->is_parked());
    }
}


bool yapvm::interpreter::ThreadManager::is_all_parked() const {
    for (Interpreter *i : interpreters_) {
        if (!i->is_parked()) {
            return false;
        }
    }
    return true;
}


void yapvm::interpreter::ThreadManager::run_all() {
    for (Interpreter *i: interpreters_) {
        i->run();
        while (i->is_parked())
            ;
    }
}

void yapvm::interpreter::ThreadManager::finish_waiting() {
    while (!join_queue_.empty()) {
        Interpreter *beg = join_queue_.front();
        join_queue_.pop_front();
        beg->join();
    }
}
