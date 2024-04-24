#include "y_objects.h"
#include "utils.h"


yapvm::yobjects::YObject::YObject(std::string type_name) : typename_{std::move(type_name)} {}


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


yapvm::yobjects::YObject *yapvm::yobjects::constr_yobject(std::string type_name) {
    return new YObject{std::move(type_name)};
}


yapvm::yobjects::YObject *yapvm::yobjects::constr_yint(ssize_t value) {}


yapvm::yobjects::YObject *yapvm::yobjects::constr_yfloat(double value) {}


yapvm::yobjects::YObject *yapvm::yobjects::constr_ystring(std::string value) {}


yapvm::yobjects::YObject *yapvm::yobjects::constr_ybool(bool value) {}


yapvm::yobjects::YObject *yapvm::yobjects::constr_ynone() {
    YObject *none_obj = new YObject("None");
    /*
     * def __eq__(self, other):
     *     return type(self) == type(other)
     */
    ast::FunctionDef *eq = new ast::FunctionDef{
        "__eq__",
        { "self", "other" },
        {

        } //TODO
    };
    none_obj->add_method("__eq__", eq);
    return none_obj;
}


yapvm::yobjects::ManagedObject::ManagedObject(YObject *value) : value_{value}, marked_{false} {}


yapvm::yobjects::YObject *yapvm::yobjects::ManagedObject::value() const { return value_; }


bool yapvm::yobjects::ManagedObject::is_marked() const { return marked_; }


void yapvm::yobjects::ManagedObject::mark() { marked_ = true; }


void yapvm::yobjects::ManagedObject::unmark() { marked_ = false; }


void yapvm::yobjects::ManagedObject::set_value(YObject *value) { value_ = value; }
