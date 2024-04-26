#pragma once
#include <string>

#include "y_objects.h"

namespace yapvm::interpreter {


enum ScopeEntryType {
    OBJECT,
    FUNCTION
    // TODO maybe class ???
};


struct ScopeEntry {
    void *value;
    ScopeEntryType type;
};


class Scope {

public:
    //TODO consstructor, destructor
    //TODO Important! Scope DO NOT manage FunctionDef and ManagedObject lifetimes, it should do GC

    bool add_object(std::string name, yobjects::ManagedObject *value);
    bool add_function(std::string signature, ast::FunctionDef *function);
    bool add(ScopeEntry entry);

    yobjects::ManagedObject *get_object(const std::string &name);
    ast::FunctionDef *get_function(const std::string &signature);
    ScopeEntry get(const std::string &name);
};

}