#include "y_objects.h"

#include "parser.h"
#include "utils.h"
#include <string>


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
        delete static_cast<std::vector<ManagedObject *> *>(___yapvm_objval_);
        return;
    }
    if (typename_ == "dict") {
        delete static_cast<KVStorage<ManagedObject *, ManagedObject *> *>(___yapvm_objval_);
        return;
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

std::vector<yapvm::yobjects::ManagedObject *> yapvm::yobjects::YObject::get_fields() {
    std::vector<ManagedObject *> res;
    std::vector<std::string *> names = get_fields_names();
    for (std::string *name : names) {
        res.push_back(get_field(*name));
    }
    return res;
}

void *yapvm::yobjects::YObject::get____yapvm_objval_() const { return ___yapvm_objval_; }


void yapvm::yobjects::YObject::set____yapvm_objval_(void *value) { ___yapvm_objval_ = value; }


bool yapvm::yobjects::YObject::get_value_as_bool() const { return *static_cast<bool *>(___yapvm_objval_); }


std::string yapvm::yobjects::YObject::get_value_as_string() const {
    return *static_cast<std::string *>(___yapvm_objval_);
}


double yapvm::yobjects::YObject::get_value_as_float() const { return *static_cast<double *>(___yapvm_objval_); }


ssize_t yapvm::yobjects::YObject::get_value_as_int() const { return *static_cast<ssize_t *>(___yapvm_objval_); }


void yapvm::yobjects::YObject::set_value_as_bool(bool value) const { *static_cast<bool *>(___yapvm_objval_) = value; }


void yapvm::yobjects::YObject::set_value_as_string(std::string value) const {
    *static_cast<std::string *>(___yapvm_objval_) = std::move(value);
}

void yapvm::yobjects::YObject::set_value_as_float(double value) const {
    *static_cast<double *>(___yapvm_objval_) = value;
}

void yapvm::yobjects::YObject::set_value_as_int(ssize_t value) const {
    *static_cast<ssize_t *>(___yapvm_objval_) = value;
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


yapvm::yobjects::YObject *yapvm::yobjects::constr_ylist(std::vector<yapvm::yobjects::ManagedObject *> *vec) {
    return new YObject{"list", vec};
}


bool yapvm::yobjects::is_collection(yapvm::yobjects::YObject * yobj) {
    if (yobj->get_typename() == "list") {
        return true;
    }
    if (yobj->get_typename() == "dict") {
        return true;
    }
    return false;
} 


std::vector<yapvm::yobjects::ManagedObject *> yapvm::yobjects::get_list_elements(yapvm::yobjects::YObject *yobj) {
    // TODO maybe add checks or hide this function
    return *static_cast<std::vector<yapvm::yobjects::ManagedObject *> *>(yobj->get____yapvm_objval_());
}

std::vector<yapvm::yobjects::ManagedObject *> yapvm::yobjects::get_dict_elements(yapvm::yobjects::YObject *yobj) {
    // TODO maybe add checks or hide this function
    KVStorage<yapvm::yobjects::ManagedObject *, yapvm::yobjects::ManagedObject *> *dict = static_cast<KVStorage<yapvm::yobjects::ManagedObject *, yapvm::yobjects::ManagedObject *> *>(yobj->get____yapvm_objval_());
    // TODO 
    std::vector<yapvm::yobjects::ManagedObject **> values = dict->get_live_entries_values();
    std::vector<yapvm::yobjects::ManagedObject **> keys = dict->get_live_entries_keys();
    std::vector<yapvm::yobjects::ManagedObject *> res{keys.size() + values.size()};

    for (yapvm::yobjects::ManagedObject **val : values) {
        res.push_back(*val);
    }
    for (yapvm::yobjects::ManagedObject **key : keys) {
        res.push_back(*key);
    }
    
    return res;
}


std::vector<yapvm::yobjects::ManagedObject *> yapvm::yobjects::get_collection_elements(yapvm::yobjects::YObject *yobj) {
    // TODO maybe add checks
    if (yobj->get_typename() == "list") {
        return get_list_elements(yobj);
    }
    if (yobj->get_typename() == "dict") {
        return get_dict_elements(yobj);
    }
    return std::vector<yapvm::yobjects::ManagedObject *> { };
}

struct __yobj_hash {
    size_t operator()(yapvm::yobjects::ManagedObject *o) const {
        return yapvm::yobjects::managed_yobject_hash(o);
    }
};


yapvm::yobjects::YObject *yapvm::yobjects::constr_ydict() {
    return new YObject{
        "dict",
        new KVStorage<
            ManagedObject *,
            ManagedObject *,
            70, 30,
            __yobj_hash
        >
    };
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


size_t yapvm::yobjects::managed_yobject_hash(ManagedObject *o) {
    YObject *value = o->value();
    assert(value != nullptr);

    const std::string &typename_ = value->get_typename();

    if (typename_ == "None") {
        return 0;
    }
    if (typename_ == "bool") {
        return std::hash<bool>{}(value->get_value_as_bool());
    }
    if (typename_ == "string") {
        return std::hash<std::string>{}(value->get_value_as_string());
    }
    if (typename_ == "float") {
        return std::hash<double>{}(value->get_value_as_float());
    }
    if (typename_ == "int") {
        return std::hash<ssize_t>{}(value->get_value_as_int());
    }
    //TODO tuples? when added

    return std::hash<size_t>{}(reinterpret_cast<size_t>(o));
}
