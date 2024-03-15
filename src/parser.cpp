#include "parser.h"
#include "ast.h"

using namespace yapvm::parser;


static
CmpOpKind *try_parse_cmp_op_kind(const std::vector<Token> &tokens, size_t &pos) {
    switch (tokens[pos].kind()) {
    case EQUAL_EQUAL:
        return new Eq{};
    case BANG_EQUAL:
        return new NotEq{};
    case LESS:
        return new Lt{};
    case LESS_EQUAL:
        return new LtE{};
    case GREATER:
        return new Gt{};
    case GREATER_EQUAL:
        return new GtE{};
    }
    return nullptr;
}


static
UnaryOpKind *try_parse_unary_op_kind(const std::vector<Token> &tokens, size_t &pos) {
    switch (tokens[pos].kind()) {
    case TILDE:
        return new Invert{};
    case BANG:
        return new Not{};
    }
    return nullptr;
}


static
BoolOpKind *try_parse_bool_op_kind(const std::vector<Token> &tokens, size_t &pos) {
    switch (tokens[pos].kind()) {
    case AND:
        return new And{};
    case OR:
        return new Or{};
    }
    return nullptr;
}


static BoolOp *try_parse_bool_op(const std::vector<Token> &tokens, size_t &pos) {
    //TODO
}


static
Expr *try_parse_expr(const std::vector<Token> &tokens, size_t &pos) {
    //TODO
}


static
OperatorKind *try_parse_operator_kind(const std::vector<Token> &tokens, size_t &pos) {
    switch (tokens[pos].kind()) {
    case PLUS:
        return new Add{};
    case MINUS:
        return new Sub{};
    case ASTERISK:
        return new Mult{};
    case SLASH:
        return new Div{};
    case MOD:
        return new Mod{};
    case DOUBLE_ASTERISK:
        return new Pow{};
    case LEFT_SHIFT:
        return new LShift{};
    case RIGHT_SHIFT:
        return new RShift{};
    case PIPE:
        return new BitOr{};
    case CARET:
        return new BitXor{};
    case AMPERSAND:
        return new BitAnd{};
    case DOUBLE_SLASH:
        return new FloorDiv{};
    }
    return nullptr;
}


static
Node *parse_node(const std::vector<Token> &tokens, size_t &pos) {
    //TODO
    return {};
}


std::vector<Node *> yapvm::parser::generate_ast(const std::vector<Token> &tokens) {
    std::vector<Node *> ast;

    size_t pos = 0;
    while (pos < tokens.size()) {
        ast.emplace_back(parse_node(tokens, pos));
    }

    return ast;
}