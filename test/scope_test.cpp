#include "gtest/gtest.h"
#include "y_objects.h"
#include "scope.h"
#include "parser.h"
#include "paths.h"

using namespace yapvm;
using namespace yobjects;
using namespace interpreter;

TEST(ScopedTest, add_object_test) {
    Scope scope;

    ManagedObject *m_int = new ManagedObject { constr_yint(42) };
    ManagedObject *string_field = new ManagedObject { constr_ystring("hello!") };
    ManagedObject *obj = new ManagedObject { constr_yobject("Greeter") };
    obj->value()->add_field("greeting", string_field);

    // x = 41
    // class Greeter:
    //    greeting = hello!
    scope.add_object("x", m_int);
    scope.add_object("Greeter", obj);

    auto obj_x = *(scope.get_object("x"));
    auto obj_greeter = *(scope.get_object("Greeter"));
    EXPECT_EQ(42, *static_cast<ssize_t *>(obj_x->value()->get____yapvm_objval_()));
    EXPECT_EQ("Greeter", obj_greeter->value()->get_typename());
}