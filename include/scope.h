#pragma once
#include <string>
#include <unordered_map>

#include "kvstorage.h"
#include "y_objects.h"

using namespace yapvm::yobjects;
using namespace yapvm::ast;

namespace yapvm::interpreter {


enum ScopeEntryType {
    OBJECT,
    FUNCTION,
    SCOPE
};


struct ScopeEntry {
    void *value_;
    ScopeEntryType type_;
};


// Scope can't manage FunctionDef and ManagedObject lifetimes; Deligate to gc
// copy constructor IF NEEDED
class Scope {
    Scope* parent_;
    KVStorage<std::string, ScopeEntry> scope_;
public:

    Scope() : parent_(nullptr) {};
    Scope(Scope* parent) : parent_(parent) {};

    const bool add_object(std::string name, ManagedObject *value);
    const bool add_function(std::string signature, FunctionDef *function);
    const bool add_child_scope(std::string name, Scope* subscope);
    const bool add(std::string name, ScopeEntry entry);

    ManagedObject *get_object(const std::string &name);
    FunctionDef *get_function(const std::string &signature);
    std::optional<ScopeEntry> get(const std::string &name);
    std::vector<Scope *> get_all_children();
};

}
