#pragma once

#include "y_objects.h"
#include "scope.h"
#include "thread_manager.h"
#include <map>
#include <string>
#include <vector>

#define GC_CASH_LIMIT 500

// TODO need to register all ManagedObject allocations
// TODO Only GC can clean ManagedObject

using namespace yapvm::yobjects;
using namespace yapvm::interpreter;

namespace yapvm::ygc {

class YGC {
    ThreadManager tm_;
    Scope *root_;
    std::vector<ManagedObject *> left_, right_;
public:
    YGC(Scope *root, const ThreadManager &tm) : root_(root), tm_(tm),
    left_(std::vector<ManagedObject *>()), right_(std::vector<ManagedObject *>()) {
        collect();
    };
private:
    void mark() const;
    void sweep() const;
    void collect() const;
};


} // namespace yapvm::ygc
