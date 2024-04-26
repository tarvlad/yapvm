#pragma once

#include "ast.h"
#include "kvstorage.h"
#include "utils.h"

/**
 * YObject
 *  YPrimitiveObject
 *   YIntObject
 *   YFloatObject
 *   YStringObject
 *   YBoolObject
 *   YNoneObject
 *  YUserObject
 *  YListObject
 *  YTupleObject
 *  YDictObject
 *  YIteratorObject
 *  HASH!!! call to interpreter needed
 */

#if _MSC_VER
static_assert(sizeof(void *) == 8); // for MSVC x64 compatibility
using ssize_t = long long;
#endif

namespace yapvm::ast {
class FunctionDef;
}

namespace yapvm::yobjects {

class ManagedObject;


class YObject {
    std::string typename_;
    KVStorage<std::string, ManagedObject *> fields_;
    KVStorage<std::string, ast::FunctionDef *> methods_;
    void *___yapvm_objval_; // reinterpret_cast for usage, yes yes very bad gcc-style type erasure
                            // chosen for performance

public:
    YObject(std::string type_name);
    YObject(std::string type_name, void *value);

    ~YObject();

    const std::string &get_typename() const { return typename_; }

    void add_field(std::string name, ManagedObject *field);

    void add_method(std::string name, yapvm::ast::FunctionDef *method);

    ManagedObject *get_field(const std::string &name);

    ast::FunctionDef *get_method(const std::string &name);

    std::vector<std::string *> get_methods_names() const;

    std::vector<std::string *> get_fields_names() const;

    void *get____yapvm_objval_() const;

    void set____yapvm_objval_(void *value);

    bool *get_value_bool() const;

    // TODO get_value_[all of primitive types] (and set_)
};


YObject *constr_yobject(std::string type_name);
YObject *constr_yint(ssize_t value);
YObject *constr_yfloat(double value);
YObject *constr_ystring(std::string value);
YObject *constr_ybool(bool value);
YObject *constr_ynone();
YObject *constr_ylist();
YObject *constr_ydict();
//TODO

class ManagedObject {
    YObject *value_;
    bool marked_;

public:
    ManagedObject(YObject *value);
    ~ManagedObject();

    YObject *value() const;
    bool is_marked() const;

    void mark();
    void unmark();
    void set_value(YObject *value);
};


size_t managed_yobject_hash(ManagedObject *o);


};

