#include "parser.h"
#include "ast.h"
#include "utils.h"
#include <cassert>

using namespace yapvm::parser;
using namespace yapvm;


/*
 * CmpOpKind    ::= EQUAL_EQUAL 
 *                  | BANG_EQUAL
 *                  | LESS
 *                  | LESS_EQUAL
 *                  | GREATER
 *                  | GREATER_EQUAL
 */
static
CmpOpKind *try_parse_cmp_op_kind(const std::vector<Token> &tokens, size_t &pos) {
    if (!is_there_at_least_n_elements_in<1>(tokens, pos)) {
        return nullptr;
    }
    switch (tokens[pos].kind()) {
    case EQUAL_EQUAL:
        pos++;
        return new Eq{};
    case BANG_EQUAL:
        pos++;
        return new NotEq{};
    case LESS:
        pos++;
        return new Lt{};
    case LESS_EQUAL:
        pos++;
        return new LtE{};
    case GREATER:
        pos++;
        return new Gt{};
    case GREATER_EQUAL:
        pos++;
        return new GtE{};
    }
    return nullptr;
}


/*
 * UnaryOpKind :== TILDE | BANG 
 */
static
UnaryOpKind *try_parse_unary_op_kind(const std::vector<Token> &tokens, size_t &pos) {
    if (!is_there_at_least_n_elements_in<1>(tokens, pos)) {
        return nullptr;
    }
    switch (tokens[pos].kind()) {
    case TILDE:
        pos++;
        return new Invert{};
    case BANG:
        pos++;
        return new Not{};
    }
    return nullptr;
}


/*
 * BoolOpKind ::= AND | OR
 */
static
BoolOpKind *try_parse_bool_op_kind(const std::vector<Token> &tokens, size_t &pos) {
    if (!is_there_at_least_n_elements_in<1>(tokens, pos)) {
        return nullptr;
    }
    switch (tokens[pos].kind()) {
    case AND:
        pos++;
        return new And{};
    case OR:
        pos++;
        return new Or{};
    }
    return nullptr;
}


static
Expr *try_parse_expr(const std::vector<Token> &tokens, size_t &pos);


/*
 * BoolOp ::= Expr BoolOpKind Expr (BoolOpKind Expr)*
 */
static 
BoolOp *try_parse_bool_op(const std::vector<Token> &tokens, size_t &pos) {
    if (!is_there_at_least_n_elements_in<3>(tokens, pos)) {
        return nullptr;
    }
    size_t prev_pos = pos;

    Expr *arg = try_parse_expr(tokens, pos);
    if (arg == nullptr) {
        pos = prev_pos;
        return nullptr;
    }
    BoolOpKind *kind = try_parse_bool_op_kind(tokens, pos);
    if (kind == nullptr) {
        pos = prev_pos;
        delete arg;
        return nullptr;
    }
    std::vector<Expr *> args;
    args.push_back(arg);
    arg = try_parse_expr(tokens, pos);
    if (arg == nullptr) {
        pos = prev_pos;
        delete args[0];
        delete kind;
        return nullptr;
    }
    args.push_back(arg);

    BoolOpKind op = *kind;
    prev_pos = pos;
    bool is_or = dynamic_cast<Or *>(kind) != nullptr;
    while (1) {
        delete kind;
        kind = try_parse_bool_op_kind(tokens, pos);
        if (kind == nullptr) {
            break;
        }
        if (is_or && dynamic_cast<Or *>(kind) == nullptr || !is_or && dynamic_cast<And *>(kind) == nullptr) {
            pos = prev_pos;
            delete kind;
            break;
        }
        arg = try_parse_expr(tokens, pos);
        if (arg == nullptr) {
            pos = prev_pos;
            delete kind;
            break;
        }
        args.push_back(arg);
        prev_pos = pos;
    }
    
    std::span<Expr *> values = { new Expr *[args.size()], args.size() };
    for (size_t i = 0; i < args.size(); i++) {
        values[i] = args[i];
    }
    return new BoolOp{ op, values };
}


static
OperatorKind *try_parse_operator_kind(const std::vector<Token> &tokens, size_t &pos);


/*
 * BinOp ::= Expr OperatorKind Expr
 */
static 
BinOp *try_parse_bin_op(const std::vector<Token> &tokens, size_t &pos) {
    if (!is_there_at_least_n_elements_in<3>(tokens, pos)) {
        return nullptr;
    }
    size_t prev_pos = pos;
    Expr *l = try_parse_expr(tokens, pos);
    if (l == nullptr) {
        pos = prev_pos;
        return nullptr;
    }
    OperatorKind *op = try_parse_operator_kind(tokens, pos);
    if (op == nullptr) {
        pos = prev_pos;
        delete l;
        return nullptr;
    }
    Expr *r = try_parse_expr(tokens, pos);
    if (r == nullptr) {
        pos = prev_pos;
        delete l;
        delete op;
        return nullptr;
    }
    OperatorKind o = *op;
    delete op;
    return new BinOp{ l, o, r };
}


/*
 * UnaryOp ::= UnaryOpKind Expr
 */
static 
UnaryOp *try_parse_unary_op(const std::vector<Token> &tokens, size_t &pos) {
    if (!is_there_at_least_n_elements_in<2>(tokens, pos)) {
        return nullptr;
    }
    size_t prev_pos = pos;
    UnaryOpKind *kind = try_parse_unary_op_kind(tokens, pos);
    if (kind == nullptr) {
        pos = prev_pos;
        return nullptr;
    }
    Expr *operand = try_parse_expr(tokens, pos);
    if (operand == nullptr) {
        pos = prev_pos;
        delete operand;
        return nullptr;
    }
    UnaryOpKind op = *kind;
    delete kind;
    return new UnaryOp{ op, operand };
}


/*
 * Dict ::= LEFT_BRACE Expr COLON Expr (COMMA Expr COLON Expr)* RIGHT_BRACE
 */
static 
Dict *try_parse_dict(const std::vector<Token> &tokens, size_t &pos) {
    if (!is_there_at_least_n_elements_in<2>(tokens, pos)) {
        return nullptr;
    }

    size_t prev_pos = pos;
    if (tokens[pos].kind() != LEFT_BRACE) {
        return nullptr;
    }
    pos++;

    Expr *k = try_parse_expr(tokens, pos);
    if (k == nullptr) {
        pos = prev_pos;
        return nullptr;
    }
    if (tokens[pos].kind() != COLON) {
        pos = prev_pos;
        delete k;
        return nullptr;
    }
    Expr *v = try_parse_expr(tokens, pos);
    if (v == nullptr) {
        pos = prev_pos;
        delete k;
        return nullptr;
    }
    if (tokens[pos].kind() == RIGHT_BRACE) {
        return new Dict{ std::span<Expr *>{ new Expr *[1] { k }, 1 }, std::span<Expr *>{ new Expr *[1] { v }, 1 } };
    }
    if (tokens[pos].kind() != COMMA) {
        pos = prev_pos;
        delete k;
        delete v;
        return nullptr;
    }

    std::vector<Expr *> keys;
    std::vector<Expr *> values;
    
    keys.push_back(k);
    values.push_back(v);
    assert(tokens[pos].kind() == COMMA);

    auto cleanup_kv = [&keys, &values] { 
        for (Expr *k : keys) { 
            delete k; 
        } 
        for (Expr *v : values) { 
            delete v; 
        } 
    };
    while (1) {
        pos++; // skip comma
        k = try_parse_expr(tokens, pos);
        if (k == nullptr) {
            pos = prev_pos;
            cleanup_kv();
            return nullptr;
        }
        if (tokens[pos].kind() != COLON) {
            pos = prev_pos;
            cleanup_kv();
            return nullptr;
        }
        pos++;
        v = try_parse_expr(tokens, pos);
        if (v == nullptr) {
            pos = prev_pos;
            cleanup_kv();
            return nullptr;
        }
        keys.push_back(k);
        values.push_back(v);
        if (tokens[pos].kind() == RIGHT_BRACE) {
            pos++;
            break;
        }
        if (tokens[pos].kind() != COMMA) {
            pos = prev_pos;
            cleanup_kv();
            return nullptr;
        }
    }
    std::span<Expr *> keys_s = { new Expr * [keys.size()], keys.size() };
    std::span<Expr *> values_s = { new Expr * [values.size()], values.size() };
    assert(keys_s.size() == values_s.size());
    for (size_t i = 0; i < keys_s.size(); i++) {
        keys_s[i] = keys[i];
        values_s[i] = values[i];
    }
    return new Dict{ keys_s, values_s };
}


/*
 * Set  ::= LEFT_BRACE Expr (COMMA Expr)* RIGHT_BRACE
 */
static 
Set *try_parse_set(const std::vector<Token> &tokens, size_t &pos) {
    if (is_there_at_least_n_elements_in<3>(tokens, pos)) {
        return nullptr;
    }
    if (tokens[pos].kind() != RIGHT_BRACE) {
        return nullptr;
    }
    pos++;
    Expr *element = try_parse_expr(tokens, pos);
    if (element == nullptr) {
        return nullptr;
    }
    std::vector<Expr *> elements;
    elements.push_back(element);

    while (1) {
        if (tokens[pos].kind() == RIGHT_BRACE) {
            pos++;
            break;
        }
        if (tokens[pos].kind() != COMMA) {
            for (Expr *e : elements) {
                delete e;
            }
            return nullptr;
        }
        pos++; // skip comma
        element = try_parse_expr(tokens, pos);
        if (element == nullptr) {
            for (Expr *e : elements) {
                delete e;
            }
            return nullptr;
        }
        elements.push_back(element);
    }
    std::span<Expr *> elements_s = { new Expr *[elements.size()], elements.size() };
    return new Set{ elements_s };
}


/*
 * OperatorKind ::= PLUS
 *                  | MINUS
 *                  | ASTERISK
 *                  | SLASH
 *                  | MOD
 *                  | DOUBLE_ASTERISK
 *                  | LEFT_SHIFT
 *                  | RIGHT_SHIFT
 *                  | PIPE
 *                  | CARET
 *                  | AMPERSAND
 *                  | DOUBLE_SLASH
 */
static
OperatorKind *try_parse_operator_kind(const std::vector<Token> &tokens, size_t &pos) {
    if (!is_there_at_least_n_elements_in<1>(tokens, pos)) {
        return nullptr;
    }
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


//TODO add spec
static
Expr *try_parse_expr(const std::vector<Token> &tokens, size_t &pos) {
    //TODO start from names and etc
}


//TODO add spec
static
Node *parse_node(const std::vector<Token> &tokens, size_t &pos) {
    //TODO
    return {};
}


std::vector<Node *> yapvm::parser::generate_ast(const std::vector<Token> &tokens) {
    std::vector<Node *> ast;

    size_t pos = 0;
    while (pos < tokens.size()) {
        ast.push_back(parse_node(tokens, pos));
    }

    return ast;
}