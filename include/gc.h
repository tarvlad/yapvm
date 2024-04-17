#pragma once

#include "y_objects.h"
#include <deque>
#include <map>
#include <string>
#include <vector>

namespace yapvm {
namespace ygc {

using namespace yapvm::yobjects;

// TODO
class YScope {
public:
    std::map<std::string, YObject*> scope_;
    YScope(const std::map<std::string, YObject*> &scope) : scope_(scope) {};
};

// TODO
class YHeap {
public:
    std::vector<YObject*> objs_;
    YHeap(const std::vector<YObject*> &objs) : objs_(objs) {};
};


class YGC {
    YScope *root_;
    YHeap *heap_;
// private:
public:
    void mark();
    void sweep();
    YGC(YScope *root, YHeap *heap) : root_(root), heap_(heap) {};
    void collect();
};


} // namespace ygc
} // namespace yapvm