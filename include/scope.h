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
    SCOPE,
    LABEL // TODO implement break, continue via labels
};


struct ScopeEntry {
    void *value_;
    ScopeEntryType type_;
};


// Scope can't manage FunctionDef and ManagedObject lifetimes; Delegate to gc
// copy constructor IF NEEDED
class Scope {
    Scope* parent_;
    KVStorage<std::string, ScopeEntry> scope_;

public:
    constexpr static const char *lst_exec_res = "__yapvm_inner_last_exec_res";
    constexpr static const char *while_loop_scope = "__yapvm_inner_while_loop_scope";
    constexpr static const char *if_scope = "__yapvm_inner_if_scope";
    constexpr static const char *function_ret_label = "__yapvm_inner_function_return_label";

    Scope();
    Scope(Scope *parent) : parent_{ parent } {};

    bool add_object(std::string name, ManagedObject *value);
    bool add_function(std::string signature, FunctionDef *function);
    bool add_child_scope(std::string name, Scope *subscope);
    bool add(std::string name, ScopeEntry entry);

    void change(const std::string &name, ScopeEntry new_entry);
    void store_last_exec_res(const std::string &name);

    ScopeEntry name_lookup(const std::string &name);

    ManagedObject *get_object(const std::string &name);
    FunctionDef *get_function(const std::string &signature);
    std::optional<ScopeEntry> get(const std::string &name);
    std::vector<Scope *> get_all_children() const;
    Scope *parent() const;
};

}
