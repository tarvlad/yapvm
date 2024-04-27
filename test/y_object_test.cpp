#include <gtest/gtest.h>

#include "parser.h"
#include "y_objects.h"
#include "paths.h"
using namespace yapvm;
using namespace yobjects;


TEST(y_object_test, None_generation) {
    YObject *obj = constr_ynone();

    EXPECT_EQ(obj->get_typename(), "None");
    EXPECT_TRUE(obj->get_methods_names().empty());
    EXPECT_TRUE(obj->get_fields_names().empty());
}


TEST(y_object_test, bool_generation) {
    YObject *obj = constr_ybool(true);

    EXPECT_EQ(obj->get_typename(), "bool");
    EXPECT_TRUE(obj->get_methods_names().empty());
    EXPECT_TRUE(obj->get_fields_names().empty());

    EXPECT_TRUE(*static_cast<bool *>(obj->get____yapvm_objval_()));
}


TEST(y_object_test, int_generation) {
    YObject *obj = constr_yint(42);

    EXPECT_EQ(obj->get_typename(), "int");
    EXPECT_TRUE(obj->get_methods_names().empty());
    EXPECT_TRUE(obj->get_fields_names().empty());

    EXPECT_TRUE(*static_cast<ssize_t *>(obj->get____yapvm_objval_()) == 42);
}


TEST(y_object_test, float_generation) {
    YObject *obj = constr_yfloat(42.0);

    EXPECT_EQ(obj->get_typename(), "float");
    EXPECT_TRUE(obj->get_methods_names().empty());
    EXPECT_TRUE(obj->get_fields_names().empty());

    EXPECT_TRUE(*static_cast<double *>(obj->get____yapvm_objval_()) == 42.0);
}


TEST(y_object_test, y_custom_test) {
    YObject *obj = constr_yobject("Test");

    EXPECT_EQ(obj->get_typename(), "Test");
    ManagedObject *val = new ManagedObject{ constr_yint(42) };
    obj->add_field("value", val);

    ast::FunctionDef *eq = checked_cast<ast::Stmt, ast::FunctionDef>(
            parser::generate_ast(trim(exec(
                "python "
                + std::string(builtin_def_paths::path_def_type_and_val_based___eq___self_other)
            )))->steal_body()[0].get(), std::terminate
    );
    obj->add_method(
        "__eq__",
        eq
    );

    EXPECT_EQ(obj->get____yapvm_objval_(), nullptr);
    EXPECT_EQ(obj->get_field("value"), val);
    EXPECT_EQ(obj->get_method("__eq__"), eq);
}
