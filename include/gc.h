#pragma once

#include "y_objects.h"
#include <deque>
#include <map>
#include <string>
#include <vector>

namespace yapvm {
namespace gc {

using namespace yapvm::yobjects;

// wrapper for YObject
class GCObject {
    YObject *obj_;
    bool is_marked_;
    std::vector<GCObject *> kids_;

public:
    GCObject(YObject *val);

    bool &is_marked();
    YObject *obj();
    std::vector<GCObject *> &kids();
};

// TODO
class Scope {
public:
    std::map<std::string, GCObject *> m_;
};

// TODO
class Heap {
public:
    std::vector<GCObject *> objs_;
};


class GC {
    Scope *root_;
    Heap *heap_;

private:
    void mark();
    void sweep();
public:
    void collect();
};



} // namespace gc
} // namespace yapvm