#include "scope.h"

using namespace yapvm::interpreter;

const bool Scope::add_object(std::string name, ManagedObject *value) {
    return scope_.add(name, ScopeEntry{static_cast<void *>(value), OBJECT});
}

const bool Scope::add_function(std::string signature, FunctionDef *function) {
    return scope_.add(signature, ScopeEntry{static_cast<void *>(function), FUNCTION}); 
}

const bool Scope::add_child_scope(std::string name, Scope* subscope) {
    subscope->parent_ = this;
    return scope_.add(name, ScopeEntry{static_cast<void *>(subscope), SCOPE}); 
}

const bool Scope::add(std::string name, ScopeEntry entry) {
    return scope_.add(name, entry);
}

ManagedObject *Scope::get_object(const std::string &name) {
    std::optional<std::reference_wrapper<ScopeEntry>> opt_from_kv = scope_[name];
    if (opt_from_kv == std::nullopt) return nullptr; 
    ScopeEntry se_ref = opt_from_kv.value().get();
    return static_cast<ManagedObject *>(se_ref.value_);
}

FunctionDef *Scope::get_function(const std::string &signature) {
    std::optional<std::reference_wrapper<ScopeEntry>> opt_from_kv = scope_[signature];
    if (opt_from_kv == std::nullopt) return nullptr; 
    ScopeEntry se_ref = opt_from_kv.value().get();
    return static_cast<FunctionDef *>(se_ref.value_);   
}

std::optional<ScopeEntry> Scope::get(const std::string &name) {
    std::optional<std::reference_wrapper<ScopeEntry>> opt_from_kv = scope_[name];
    if (opt_from_kv == std::nullopt) return std::nullopt; 
    ScopeEntry se_ref = opt_from_kv.value().get();
    return std::optional<ScopeEntry>{ se_ref };
}

std::vector<Scope *> Scope::get_all_children() {
    std::vector<Scope *> children;
    auto fields = scope_.get_live_entries_values();
    for (ScopeEntry *f : fields) {
        if (f->type_ == SCOPE) children.push_back(static_cast<Scope *>(f->value_));
    }
    return children;
}

Scope* Scope::parent() const {
    return parent_;
}
