#pragma once

#include "y_objects.h"
#include "scope.h"
#include "thread_manager.h"
#include <string>
#include <vector>

#define GC_CASH_LIMIT 5000

// TODO need to register all ManagedObject allocations
// TODO Only GC can clean ManagedObject

using namespace yapvm::yobjects;
using namespace yapvm::interpreter;

namespace yapvm::ygc {

class YGC {
    ThreadManager *tm_;
    Scope *root_;
    std::vector<ManagedObject *> left_{};
    std::vector<ManagedObject *> right_{};

    bool need_check_hs_ = false;
    size_t max_hs_ = -1;
public:
    YGC(Scope *root, ThreadManager *tm) : root_(root), tm_(tm),
    left_(std::vector<ManagedObject *>()), right_(std::vector<ManagedObject *>()) {};

    YGC(Scope *root, ThreadManager *tm, size_t max_hs) : root_(root), tm_(tm),
    left_(std::vector<ManagedObject *>()), right_(std::vector<ManagedObject *>()),
          max_hs_(max_hs), need_check_hs_(true) {};

    void mark();
    void sweep();
    void collect();

    // TODO function only for testing, will be deprecated
    void fill_left(std::vector<ManagedObject *> &);
    std::vector<ManagedObject *> &left();
};


} // namespace yapvm::ygc
