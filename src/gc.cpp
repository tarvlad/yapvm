#include "gc.h"
#include <chrono>
#include <deque>
#include <thread>
#include <unordered_set>
#include <iostream>

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
    size_t counter = root_objects.size();
    Logger::log("GC", "mark", "found " + std::to_string(counter) + " root objects");

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
                counter++;
                kid->mark();
                deq.push_back(kid);
            }
        }
    }
    Logger::log("GC", "mark", "marked " + std::to_string(counter) + " live objects");
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
    size_t deleted_ctr = 0;
    assert(is_unique(left_));
    for (ManagedObject *obj : left_) {
        if (!obj->is_marked()) {
            delete(obj);
            deleted_ctr++;
            if (need_check_hs_) max_hs_ += sizeof(ManagedObject);
        } else {
            obj->unmark();
            right_.push_back(obj);
        }
    }
    left_.swap(right_);
    right_ = {};

    Logger::log("GC", "sweep", "deleted " + std::to_string(deleted_ctr) + " dead objects");
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
        while (!tm_->park_all());
        std::optional<bool> is_all_parked_opt = tm_->is_all_parked();
        while (true) {
            if (is_all_parked_opt.has_value()) {
                if (!is_all_parked_opt.value()) {
                    sleepns(1000);
                } else {
                    break;
                }
            }
            is_all_parked_opt = tm_->is_all_parked();
        }
        Logger::log("GC", "all threads parked");

        std::vector<Interpreter *> interprets;
        std::optional opt_interprets = tm_->get_all_interpreters();
        while (true) {
            if (opt_interprets.has_value()) {
                interprets = opt_interprets.value();
                break;
            }
            opt_interprets = tm_->get_all_interpreters();
        }
        Logger::log("GC", "get " + std::to_string(interprets.size()) + " live interpreters");
        if (interprets.empty()) {
            Logger::log("GC", "found no live interpreters, finishing...");
            while (!tm_->finish_waiting()); // join
            Logger::log("GC", "all joins finished, exiting...");
            break;
        }

        Logger::log("GC", "registering allocated objects");
        for (Interpreter *i : interprets) {
            std::vector<ManagedObject *> register_queue = i->get_register_queue();
            for (ManagedObject *mo : register_queue) {
                if (need_check_hs_) {
                    if (mo->value()->get_typename() == "string") {
                        max_hs_ -= static_cast<size_t>(mo->value()->get_value_as_string().size());
                    }
                    max_hs_ -= sizeof(ManagedObject);
                    if (max_hs_ < 0) {
                        throw std::runtime_error("GC: max heap size exceeded");
                    }
                }

                left_.push_back(mo);
            }
        }
        
        if (left_.size() >= GC_CASH_LIMIT) {
            Logger::log("GC", "GC_CASH_LIMIT over, mark and sweep started...");
            Logger::log("GC","in heap now " + std::to_string(left_.size()) + " objects");
            mark();
            sweep();
        }

        Logger::log("GC", "gc cycle end, running all threads...");
        while (!tm_->run_all());

        sleepms(1000);
    }
}



