#include "scope.h"

using namespace yapvm::interpreter;

const bool Scope::add_object(std::string name, ManagedObject *value) {
    return scope_.add(name, ScopeEntry{static_cast<ManagedObject *>(value), OBJECT});
}

const bool Scope::add_function(std::string signature, FunctionDef *function) {
    return scope_.add(signature, ScopeEntry{static_cast<FunctionDef *>(function), OBJECT}); 
}

const bool Scope::add(std::string name, ScopeEntry entry) {
    return scope_.add(name, entry);
}

std::optional<ManagedObject *> Scope::get_object(const std::string &name) {
    std::optional<std::reference_wrapper<ScopeEntry>> opt_from_kv = scope_[name];
    if (opt_from_kv == std::nullopt) return std::nullopt; 
    ScopeEntry se_ref = opt_from_kv.value().get();
    return std::optional<ManagedObject *> {static_cast<ManagedObject *>(se_ref.value_)};
}

std::optional<FunctionDef *> get_function(const std::string &signature) {
    // std::optional<std::reference_wrapper<ScopeEntry>> opt_from_kv = scope_[name];
    // if (opt_from_kv == std::nullopt) return std::nullopt; 
    // ScopeEntry se_ref = opt_from_kv.value().get();
    // return std::optional<ManagedObject *> {static_cast<ManagedObject *>(se_ref.value_)};    
}
    // ScopeEntry get(const std::string &name);