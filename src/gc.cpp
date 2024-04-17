#include "gc.h"

using namespace yapvm::ygc;

void YGC::mark() {
    std::deque<YObject*> deq;
    for (const auto &[key, val] : root_->scope_) {
        val->is_marked() = true;
        deq.push_back(val);
    }

    while (!deq.empty()) {
        YObject *curr_obj = deq.front();
        deq.pop_front();
        YIterator *it = curr_obj->iter();
        if (!it) continue;
        while (it->has_next()) {
            YObject &child = *(*it);
            if (!child.is_marked()) {
                child.is_marked() = true;
                deq.push_back(&child);
            }
            it->next();
        }
        delete it;
    }
}

void YGC::sweep() {
    for (auto obj : heap_->objs_) {
        if (!obj->is_marked()) {
            delete(obj);
        } else {
            obj->is_marked() = false;
        }
    }
}

void YGC::collect() {
    mark();
    sweep();
}



