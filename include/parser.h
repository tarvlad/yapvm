#pragma once

#include "ast.h"

namespace yapvm {
namespace parser {

using namespace yapvm::ast;

std::vector<Node *> generate_ast(const std::string &input);

} // namespace parser
} // namespace yapvm