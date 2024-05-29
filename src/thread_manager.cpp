#include "thread_manager.h"
#include "interpreter.h"

//TODO park_all taking monitor and unregister cannot happen
//TODO make methods return bool and try -> handle safepoint -> try in interpreter

bool yapvm::interpreter::ThreadManager::register_interpreter(Interpreter *interpreter) {
    if (std::unique_lock lock = std::unique_lock{ monitor_, std::try_to_lock}) {
        interpreters_.push_back(interpreter);
        return true;
    }
    return false;
}


bool yapvm::interpreter::ThreadManager::unregister_interpreter(Interpreter *interpreter) {
    if (std::unique_lock lock = std::unique_lock{ monitor_, std::try_to_lock}) {
        for (size_t i = 0; i < interpreters_.size(); i++) {
            if (interpreters_[i] == interpreter) {
                std::vector<Interpreter *>::iterator it = interpreters_.begin();
                std::advance(it, i);
                interpreters_.erase(it);
                break;
            }
        }
        join_queue_.push_back(interpreter);
        return true;
    }
    return false;
}


std::optional<std::vector<yapvm::interpreter::Interpreter *>> yapvm::interpreter::ThreadManager::get_all_interpreters() {
    if (std::unique_lock lock = std::unique_lock{ monitor_, std::try_to_lock}) {
        return interpreters_;
    }
    return std::nullopt;
}


bool yapvm::interpreter::ThreadManager::park_all() {
    if (std::unique_lock lock = std::unique_lock{ monitor_, std::try_to_lock}) {
        for (Interpreter *interpreter : interpreters_) {
            interpreter->park();
        }
        for (Interpreter *interpreter : interpreters_) {
            while (!interpreter->is_parked());
        }
        return true;
    }
    return false;
}


std::optional<bool> yapvm::interpreter::ThreadManager::is_all_parked() {
    if (std::unique_lock lock = std::unique_lock{ monitor_, std::try_to_lock}) {
        for (Interpreter *i : interpreters_) {
            if (!i->is_parked()) {
                return false;
            }
        }
        return true;
    }
    return std::nullopt;
}


bool yapvm::interpreter::ThreadManager::run_all() {
    if (std::unique_lock lock = std::unique_lock{ monitor_, std::try_to_lock}) {
        for (Interpreter *i: interpreters_) {
            i->run();
            while (i->is_parked());
        }
        return true;
    }
    return false;
}


bool yapvm::interpreter::ThreadManager::finish_waiting() {
    if (std::unique_lock lock = std::unique_lock{ monitor_, std::try_to_lock}) {
        while (!join_queue_.empty()) {
            Interpreter *beg = join_queue_.front();
            join_queue_.pop_front();
            beg->join();
            delete beg;
        }
        return true;
    }
    return false;
}


std::optional<bool> yapvm::interpreter::ThreadManager::is_registered(Interpreter *interpreter) {
    if (std::unique_lock lock = std::unique_lock{ monitor_, std::try_to_lock}) {
        for (Interpreter *i : interpreters_) {
            if (i == interpreter) {
                return true;
            }
        }
        return false;
    }
    return std::nullopt;
}
