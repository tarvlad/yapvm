#include <iostream>
#include "gtest/gtest.h"
#include <vector>
#include "y_objects.h"
#include "scope.h"
#include "parser.h"
#include "paths.h"

using namespace yapvm;
using namespace yobjects;
using namespace interpreter;

size_t dfs_scope(Scope* node) {
    std::vector<Scope*> subscopes = node->get_all_children(); 
    size_t res = 0;
    for(Scope* s : subscopes) {
        res += dfs_scope(s);
    }
    return res + 1;
}

TEST(scope_test, add_object_test) {
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

    // test
    EXPECT_EQ(42, *static_cast<ssize_t *>(scope.get_object("x")->value()->get____yapvm_objval_()));
    EXPECT_EQ("Greeter", scope.get_object("Greeter")->value()->get_typename());
}

TEST(scope_test, add_function) {
    Scope scope;   

    // def foo():
    //     return "foo"

    ast::FunctionDef *foo = checked_cast<ast::Stmt, ast::FunctionDef>(
        parser::generate_ast(trim(exec(
            "python "
            + std::string(builtin_def_paths::path_def_simple_function)
        )))->steal_body()[0].get(), std::terminate
    );

    // I dunno if this a good idea to check signature
    scope.add_function("foo", foo);
    EXPECT_TRUE(nullptr != scope.get_function("foo"));
}

TEST(scope_test, subscopes) {
    Scope main_scope;    
    Scope loop_scope;
    Scope for_scope;
    Scope inner_scope;

    // main_scope
    //     |for_scope
    //     |loop_scope
    //         |inner_scope

    // inner scope
    inner_scope.add_object("greeting", new ManagedObject { constr_ystring("hello!") });

    // loop
    loop_scope.add_object("i", new ManagedObject { constr_yint(0) });
    loop_scope.add_child_scope("inner", &inner_scope);

    // for loop
    for_scope.add_object("i", new ManagedObject { constr_yint(0) } );

    // main scope
    main_scope.add_child_scope("loop", &loop_scope);
    main_scope.add_child_scope("for", &for_scope);


    // test
    auto subscopes = main_scope.get_all_children();
    
    EXPECT_EQ(2, subscopes.size());

    size_t all_scopes = dfs_scope(&main_scope);
    EXPECT_EQ(4, all_scopes);
}
