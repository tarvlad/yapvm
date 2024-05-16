#pragma once
#include <deque>
#include <mutex>
#include <optional>
#include <vector>

namespace yapvm::interpreter {
class Interpreter;

// not thread-safe, there can be only one instance in gc
class ThreadManager {
    std::mutex monitor_;
    std::vector<Interpreter *> interpreters_;
    std::deque<Interpreter *> join_queue_;

public:
    bool register_interpreter(Interpreter *interpreter);
    bool unregister_interpreter(Interpreter *interpreter);

    std::optional<std::vector<Interpreter *>> get_all_interpreters();
    bool park_all();
    std::optional<bool> is_all_parked();
    bool run_all();

    bool finish_waiting();
    std::optional<bool> is_registered(Interpreter *);
};

}
