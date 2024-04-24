#include "ast.h"
#include "parser.h"
#include "utils.h"
#include <gtest/gtest.h>

using namespace yapvm::ast;
using namespace yapvm;
using namespace yapvm::parser;


TEST(parser_test, module_gen) {
    std::string module_def = "Module(body=[], type_ignores=[])";
    scoped_ptr<Module> module = generate_ast(module_def);

    EXPECT_EQ(module->body().size(), 0);
}

TEST(parser_test, import_gen) {
    std::string module_def = "Module(body=[Import(names=[alias(name='some.name')])], type_ignores=[])";
    scoped_ptr<Module> module = generate_ast(module_def);

    EXPECT_EQ(module->body().size(), 1);

    Import *import = checked_cast<Stmt, Import>(module->body()[0].get(), std::terminate);
    EXPECT_EQ(import->name(), "some.name");
}


/**
 * def __eq__(self, other):
 *     return type(self) == type(other)
 */
TEST(parser_test, type_based_eq_gen) {
    std::string eq_def = trim(exec("python test_resources/none_eq.py"));
    scoped_ptr<Module> module = generate_ast(eq_def);

    EXPECT_EQ(module.get()->body().size(), 1);
    FunctionDef *eq = checked_cast<Stmt, FunctionDef>(module->body()[0].get(), std::terminate);

    EXPECT_EQ(eq->name(), "__eq__");
    EXPECT_EQ(eq->args()[0], "self");
    EXPECT_EQ(eq->args()[1], "other");

    const std::vector<scoped_ptr<Stmt>> &body = eq->body();

    EXPECT_EQ(body.size(), 1);
    Return *return_ = checked_cast<Stmt, Return>(body[0], std::terminate);

    EXPECT_EQ(return_->returns_anything(), true);
    Compare *compare = checked_cast<Expr, Compare>(return_->value(), std::terminate);

    Call *call = checked_cast<Expr, Call>(compare->left(), std::terminate);
    Name *func = checked_cast<Expr, Name>(call->func(), std::terminate);
    EXPECT_EQ(func->id(), "type");
    EXPECT_EQ(typeid(*func->ctx()), typeid(Load));

    EXPECT_EQ(call->args().size(), 1);
    func = checked_cast<Expr, Name>(call->args()[0], std::terminate);
    EXPECT_EQ(func->id(), "self");
    EXPECT_EQ(typeid(*func->ctx()), typeid(Load));

    EXPECT_EQ(compare->comparators().size(), 1);
    call = checked_cast<Expr, Call>(compare->comparators()[0], std::terminate);
    EXPECT_EQ(call->args().size(), 1);
    func = checked_cast<Expr, Name>(call->args()[0], std::terminate);
    EXPECT_EQ(func->id(), "other");
    EXPECT_EQ(typeid(*func->ctx()), typeid(Load));
}