#pragma once

#include "lexer.h"
#include "ast.h"

namespace yapvm {
namespace parser {

using namespace yapvm::ast;

std::vector<Node *> generate_ast(const std::vector<Token> &tokens);

} // namespace parser
} // namespace yapvm