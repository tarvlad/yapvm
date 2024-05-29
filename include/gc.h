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
    ThreadManager *tm_;
    Scope *root_;
    std::vector<ManagedObject *> left_{};
    std::vector<ManagedObject *> right_{};
#ifdef MAX_HS
    size_t hs_count = 0;
#endif
public:
    YGC(Scope *root, ThreadManager *tm) : root_(root), tm_(tm),
    left_(std::vector<ManagedObject *>()), right_(std::vector<ManagedObject *>()) {
   //     collect();
    };
    ~YGC() noexcept {
        mark();
        sweep();
    };
//private:
    void mark();
    void sweep();
    void collect();
    // TODO function inly for testing, will be deprecated
    void fill_left(std::vector<ManagedObject *> &);
    std::vector<ManagedObject *> &left();
};


} // namespace yapvm::ygc
