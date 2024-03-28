#pragma once

#include "ast.h"
#include "utils.h"


namespace yapvm {
namespace parser {

using namespace yapvm::ast;

scoped_ptr<Module> generate_ast(const std::string &input);

} // namespace parser
} // namespace yapvm