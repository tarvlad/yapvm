#include "y_objects.h"

#include <paths.h>

#include "parser.h"
#include "utils.h"


yapvm::yobjects::YObject::YObject(std::string type_name) : typename_{std::move(type_name)}, ___yapvm_objval_{ nullptr } {}


yapvm::yobjects::YObject::YObject(std::string type_name, void *value) : typename_{ std::move(type_name) }, ___yapvm_objval_{ value } {}


yapvm::yobjects::YObject::~YObject() {
    if (___yapvm_objval_ == nullptr) {
        return;
    }
    if (typename_ == "bool") {
        delete static_cast<bool *>(___yapvm_objval_);
        return;
    }
    if (typename_ == "int") {
        delete static_cast<int *>(___yapvm_objval_);
        return;
    }
    if (typename_ == "float") {
        delete static_cast<double *>(___yapvm_objval_);
        return;
    }
    if (typename_ == "string") {
        delete static_cast<std::string *>(___yapvm_objval_);
        return;
    }
    if (typename_ == "list") {
        std::terminate();
        // TODO get vector and delete all elements
    }
    if (typename_ == "dict") {
        std::terminate();
        // TODO get KVStorage and delete all elements
    }
}


void yapvm::yobjects::YObject::add_field(std::string name, ManagedObject *field) {
    fields_.add(std::move(name), field);
}


void yapvm::yobjects::YObject::add_method(std::string name, ast::FunctionDef *method) {
    methods_.add(std::move(name), method);
}


yapvm::yobjects::ManagedObject *yapvm::yobjects::YObject::get_field(const std::string &name) {
    using kv_value_t = std::reference_wrapper<ManagedObject *>;
    if (std::optional<kv_value_t> field = fields_[name]; field.has_value()) {
        return field.value().get();
    }
    return nullptr;
}


yapvm::ast::FunctionDef *yapvm::yobjects::YObject::get_method(const std::string &name) {
    using kv_value_t = std::reference_wrapper<ast::FunctionDef *>;
    if (std::optional<kv_value_t> field = methods_[name]; field.has_value()) {
        return field.value().get();
    }
    return nullptr;
}


std::vector<std::string *> yapvm::yobjects::YObject::get_methods_names() const {
    return methods_.get_live_entries_keys();
}


std::vector<std::string *> yapvm::yobjects::YObject::get_fields_names() const {
    return fields_.get_live_entries_keys();
}


void *yapvm::yobjects::YObject::get____yapvm_objval_() const { return ___yapvm_objval_; }


void yapvm::yobjects::YObject::set____yapvm_objval_(void *value) {
    ___yapvm_objval_ = value;
}


yapvm::yobjects::YObject *yapvm::yobjects::constr_yobject(std::string type_name) {
    return new YObject{std::move(type_name)};
}


yapvm::yobjects::YObject *yapvm::yobjects::constr_yint(ssize_t value) {
    return new YObject{"int", new ssize_t{ value }};
}


yapvm::yobjects::YObject *yapvm::yobjects::constr_yfloat(double value) {
    return new YObject{ "float", new double{ value }};
}


yapvm::yobjects::YObject *yapvm::yobjects::constr_ystring(std::string value) {
    return new YObject{ "string", new std::string{ std::move(value) }};
}


yapvm::yobjects::YObject *yapvm::yobjects::constr_ybool(bool value) {
    return new YObject{ "bool", new bool{ value }};
}


yapvm::yobjects::YObject *yapvm::yobjects::constr_ynone() { return new YObject{"None"}; }


yapvm::yobjects::YObject *yapvm::yobjects::constr_ylist() {
    return new YObject{"list", new std::vector<ManagedObject *>{}};
}


yapvm::yobjects::YObject *yapvm::yobjects::constr_ydict() {
    return new YObject{ "dict", new std::vector<ManagedObject *>{} };
}


yapvm::yobjects::ManagedObject::ManagedObject(YObject *value) : value_{value}, marked_{false} {}


yapvm::yobjects::ManagedObject::~ManagedObject() {
    delete value_;
}


yapvm::yobjects::YObject *yapvm::yobjects::ManagedObject::value() const { return value_; }


bool yapvm::yobjects::ManagedObject::is_marked() const { return marked_; }


void yapvm::yobjects::ManagedObject::mark() { marked_ = true; }


void yapvm::yobjects::ManagedObject::unmark() { marked_ = false; }


void yapvm::yobjects::ManagedObject::set_value(YObject *value) { value_ = value; }
