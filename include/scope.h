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
    FUNCTION
    //SCOPE?
};


struct ScopeEntry {
    void *value_;
    ScopeEntryType type_;
};


// Scope can't manage FunctionDef and ManagedObject lifetimes; Deligate to gc
class Scope {
    KVStorage<std::string, ScopeEntry> scope_;
public:
    //TODO copy constructor

    const bool add_object(std::string name, ManagedObject *value);
    const bool add_function(std::string signature, FunctionDef *function);
    const bool add(std::string name, ScopeEntry entry);

    std::optional<ManagedObject *> get_object(const std::string &name);
    std::optional<FunctionDef *> get_function(const std::string &signature);
    ScopeEntry get(const std::string &name);
};

}