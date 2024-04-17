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

    Import *import = dynamic_cast<Import *>(module->body()[0].get());
    EXPECT_NE(import, nullptr);
    EXPECT_EQ(import->name(), "some.name");
}