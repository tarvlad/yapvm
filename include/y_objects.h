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

namespace yapvm::yobjects {;

class ManagedObject;

class YObject {
    std::string typename_;
    KVStorage<std::string, ManagedObject *> fields_;
    KVStorage<std::string, ast::FunctionDef *> methods_;

public:
    YObject(std::string type_name);
    ~YObject() = default;

    const std::string &get_typename() const { return typename_; }

    void add_field(std::string name, ManagedObject *field);

    void add_method(std::string name, yapvm::ast::FunctionDef *method);

    ManagedObject *get_field(const std::string &name);

    yapvm::ast::FunctionDef *get_method(const std::string &name);
};


YObject *constr_yobject(std::string type_name);
YObject *constr_yint(ssize_t value);
YObject *constr_yfloat(double value);
YObject *constr_ystring(std::string value);
YObject *constr_ybool(bool value);
YObject *constr_ynone();
//TODO

class ManagedObject {
    YObject *value_;
    bool marked_;

public:
    ManagedObject(YObject *value);

    YObject *value() const;
    bool is_marked() const;

    void mark();
    void unmark();
    void set_value(YObject *value);
};


};

