#include "gc.h"
#include <chrono>
#include <deque>
#include <thread>
#include <unordered_set>

#include "interpreter.h"
#include "logger.h"

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

static bool is_unique(const std::vector<ManagedObject *> &muobj) {
    std::unordered_set<ManagedObject *> objs;
    for (ManagedObject *m : muobj) {
        if (objs.contains(m)) {
            return false;
        }
        objs.insert(m);
    }
    std::unordered_set<YObject *> objs_y;
    for (ManagedObject *m : muobj) {
        if (objs_y.contains(m->value())) {
            return false;
        }
        objs_y.insert(m->value());
    }
    return true;
}

void YGC::sweep() {
    assert(is_unique(left_));
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
        Logger::log("GC", "gc cycle started, parking all threads...");
        tm_->park_all();
        while (!tm_->is_all_parked()) {
            sleepns(1000);
        }
        Logger::log("GC", "all threads parked");

        std::vector<Interpreter *> interprets = tm_->get_all_interpreters();
        Logger::log("GC", "get " + std::to_string(interprets.size()) + " live interpreters");
        if (interprets.empty()) {
            Logger::log("GC", "found no live interpreters, finishing...");
            tm_->finish_waiting(); // join
            Logger::log("GC", "all joins finished, exiting...");
            break;
        }

        Logger::log("GC", "registering allocated objects");
        for (Interpreter *i : interprets) {
            std::vector<ManagedObject *> register_queue = i->get_register_queue();
            for (ManagedObject *mo : register_queue) {
                left_.push_back(mo);
            }
        }
        
        if (left_.size() >= GC_CASH_LIMIT) {
            Logger::log("GC", "GC_CASH_LIMIT over, mark and sweep started...");
            mark();
            sweep();
        }

        Logger::log("GC", "gc cycle end, running all threads...");
        tm_->run_all();

        sleepms(1000);
    }
}



