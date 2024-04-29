#include "gc.h"
#include <deque>
#include <thread>
#include <chrono>

#define sleepns(val) std::this_thread::sleep_for(std::chrono::nanoseconds(val));
#define sleepms(val) std::this_thread::sleep_for(std::chrono::milliseconds(val));

using namespace yapvm::ygc;

void YGC::mark() {
    std::deque<ManagedObject *> deq;
    // all root objects, no nullptr's here 
    std::vector<ManagedObject *> root_objects = root_->get_all_objects();
    for (ManagedObject *obj : root_objects) {
        obj->mark();
        deq.push_back(obj);
    }

    while (!deq.empty()) {
        ManagedObject *curr_obj = deq.front();
        deq.pop_front();
        std::vector<ManagedObject *> kids;
        if (is_collection(curr_obj->value())) {
            kids = get_collection_elements(curr_obj->value());
        } else {
            kids = curr_obj->value()->get_fields();
        }
        if (kids.empty()) continue;
        for (ManagedObject *kid : kids) {
            if (!kid->is_marked()) {
                kid->mark();
                deq.push_back(kid);
            }
        }
    }
}


void YGC::sweep() {
    for (ManagedObject *obj : left_) {
        if (!obj->is_marked()) {
            delete(obj);
        } else {
            obj->unmark();
            right_.push_back(obj);
        }
    }
   left_.swap(right_);
   right_.clear();
}

void YGC::fill_left(std::vector<ManagedObject *> &vec) {
    for (ManagedObject *obj : vec) {
        left_.push_back(obj);
    }
}

std::vector<ManagedObject *> &YGC::left() {
    return left_;
}

void YGC::collect() {
    while (true) {
        // tm_.park_all();
        // while (!tm_.is_all_parked()) {
        //     sleepns(500);
        // }

        // std::vector<Interpreter *> interprets = tm_.get_all_interpreters();

        // if (interprets.size() == 0) {
        //     // tm_.run_all();
        //     break;
        // }

        // for (Interpreter * i : interprets) {
        //     // left_.append_range(i->get_register_queue());
        // }
        
        if (left_.size() >= GC_CASH_LIMIT) {
            mark();
            sweep();
        }
        
        // tm_.run_all();

        sleepms(500);
    }
}



