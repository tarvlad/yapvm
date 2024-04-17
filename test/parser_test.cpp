#include "ast.h"
#include "parser.h"
#include "utils.h"
#include "ast_test_data.h"
#include <gtest/gtest.h>

using namespace yapvm::ast;
using namespace yapvm;
using namespace yapvm::parser;


TEST(parser_test, ast_gen) {
    std::string test_data = ast_txt;
    scoped_ptr<Module> ast = generate_ast(test_data);
}