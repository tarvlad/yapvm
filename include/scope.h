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
    LABEL,
    THREAD_SCOPE // TODO implement break, continue via labels
};


struct ScopeEntry {
    void *value_;
    ScopeEntryType type_;
};

bool operator==(const ScopeEntry &a, const ScopeEntry &b);


// Scope can't manage FunctionDef and ManagedObject lifetimes; Delegate to gc
// copy constructor IF NEEDED
class Scope {
    Scope* parent_;
    KVStorage<std::string, ScopeEntry> scope_;

public:
    constexpr static const char *lst_exec_res = "__yapvm_inner_last_exec_res";
    constexpr static const char *yapvm_thread_func_name = "__yapvm_thread";
    constexpr static const char *yapvm_thread_join_func_name = "__yapvm_thread_join";

    Scope();
    Scope(Scope *parent) : parent_{ parent } {};

    bool add_object(std::string name, ManagedObject *value);
    bool add_function(std::string signature, FunctionDef *function);
    bool add_child_scope(std::string name, Scope *subscope);
    bool add(std::string name, ScopeEntry entry);

    void change(const std::string &name, ScopeEntry new_entry);
    void del(const std::string &name);
    void store_last_exec_res(const std::string &name);
    void update_last_exec_res(ManagedObject *value);

    ScopeEntry name_lookup(const std::string &name);

    static std::string scope_entry_function_name(const std::string &name);
    static std::string scope_entry_call_subscope_name(const std::string &name);
    static std::string scope_entry_thread_name(size_t id);

    ManagedObject *get_object(const std::string &name);
    FunctionDef *get_function(const std::string &signature);
    std::optional<ScopeEntry> get(const std::string &name);
    std::vector<Scope *> get_all_children() const;
    std::vector<ManagedObject*> get_all_objects() const;
    std::vector<std::pair<std::string, ScopeEntry>> get_all() const;
    Scope *parent() const;
};

}
