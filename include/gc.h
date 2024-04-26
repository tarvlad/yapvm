#pragma once

#include "y_objects.h"
#include <map>
#include <string>
#include <vector>


// TODO need to register all ManagedObject allocations
// TODO Only GC can clean ManagedObject
// TODO add function ManagedObject *constr(YObject *value) function to GC, which only can be used
// TODO for create ManagedObject-s. After this, owner of value is constructed ManagedObject

namespace yapvm::ygc {

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
public:
    void mark() const;
    void sweep() const;
    YGC(YScope *root, YHeap *heap) : root_(root), heap_(heap) {};
    void collect() const;
};


} // namespace yapvm::ygc
