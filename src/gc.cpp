#include "gc.h"

using namespace yapvm::gc;

GCObject::GCObject(YObject *obj)
    : obj_(obj), is_marked_(false) {}

bool &GCObject::is_marked() {
    return is_marked_;
}

YObject *GCObject::obj() {
    return obj_;
}

std::vector<GCObject *> &GCObject::kids() {
    return kids_;
}

void GC::mark() {
    std::deque<GCObject *> deq;
    for (const auto &[key, val] : root_->m_) {
        val->is_marked() = true;
        deq.push_back(val);
    }

    while (!deq.empty()) {
        GCObject *curr_obj = deq.front();
        deq.pop_front();
        auto kids = curr_obj->kids();
        for (GCObject *child : kids) {
            if (!child->is_marked()) {
                child->is_marked() = true;
                deq.push_back(child);
            }
        }
    }
}

void GC::sweep() {
    for (auto obj : heap_->objs_) {
        if (!obj->is_marked()) {
            delete(obj);
        } else {
            obj->is_marked() = false;
        }
    }
}

void GC::collect() {
    mark();
    sweep();
}


