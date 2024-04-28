#include "gc.h"
#include <deque>
#include <thread>
#include <chrono>

#define sleepns(val) std::this_thread::sleep_for(std::chrono::nanoseconds(val));
#define sleepms(val) std::this_thread::sleep_for(std::chrono::milliseconds(val));

using namespace yapvm::ygc;

// void YGC::mark() const {
//     std::deque<YObject*> deq;
//    for (const auto &[key, val] : root_->scope_) {
//        val->is_marked() = true;
//        deq.push_back(val);
//    }

//    while (!deq.empty()) {
//        YObject *curr_obj = deq.front();
//        deq.pop_front();
//        YIterator *it = curr_obj->iter();
//        if (!it) continue;
//        while (it->has_next()) {
//            YObject &child = *(*it);
//            if (!child.is_marked()) {
//                child.is_marked() = true;
//                deq.push_back(&child);
//            }
//            it->next();
//        }
//        delete it;
//    }
// }

//
//void YGC::sweep() const {
//    for (auto obj : heap_->objs_) {
//        if (!obj->is_marked()) {
//            delete(obj);
//        } else {
//            obj->is_marked() = false;
//        }
//    }
//}
//
// [v1 v2 v3] [v1 v3]
// [v1 v3] []

void YGC::collect() const {
    while (true) {
        tm_.park_all();
        while (!tm_.is_all_parked()) {
            sleepns(500);
        }

        std::vector<Interpreter *> interprets = tm.get_all_interpreters();

        if (interpreters.size() == 0) {
            tm_.run_all();
            break;
        }

        for (Interpreter * i : interpreters) {
            left_.append_range(i->get_register_queue());
        }
        
        if (left_.size() >= GC_CASH_LIMIT) {
            mark();
            sweep();
        }
        
        tm_.run_all();

        sleepms(500);
    }
}



