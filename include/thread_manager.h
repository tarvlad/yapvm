#pragma once
#include <deque>
#include <mutex>
#include <vector>

namespace yapvm::interpreter {
class Interpreter;

// not thread-safe, there can be only one instance in gc
class ThreadManager {
    std::mutex monitor_;
    std::vector<Interpreter *> interpreters_;
    std::deque<Interpreter *> join_queue_;

public:
    void register_interpreter(Interpreter *interpreter); //TODO add mutex
    void unregister_interpreter(Interpreter *interpreter);

    std::vector<Interpreter *> get_all_interpreters();
    void park_all();
    bool is_all_parked();
    void run_all();

    void finish_waiting();
    bool is_registered(Interpreter *);
};

}
