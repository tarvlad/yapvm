#include "parser.h"
#include "utils.h"
#include "ast.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <cassert>


using namespace yapvm::parser;
using namespace yapvm::ast;
using namespace yapvm;


static void parse_error(size_t pos, std::string file, std::string line) {
    throw std::runtime_error("Error while module generation at pos [" + std::to_string(pos) + "], throwed at " + file + " on line " + line);
}


static
scoped_ptr<Stmt> generate_stmt(const std::string &input, size_t &pos);


static
scoped_ptr<Expr> generate_expr(const std::string &input, size_t &pos);


static 
array<scoped_ptr<Stmt>> generate_stmt_array(const std::string &input, size_t &pos) {
    assume(input[pos] == '[', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    if (input[pos] == ']') {
        pos++;
        return {};
    }

    std::vector<scoped_ptr<Stmt>> statements;
    while (true) {
        scoped_ptr<Stmt> stmt = generate_stmt(input, pos);
        statements.emplace_back(std::move(stmt));
        if (input[pos] == ',' && input[pos + 1] == ' ') {
            pos += 2;
            continue;
        }
        break;
    }
    array<scoped_ptr<Stmt>> result = std::move(statements);
    assume(input[pos] == ']', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return result;
}


static
array<scoped_ptr<Expr>> generate_expr_array(const std::string &input, size_t &pos) {
    assume(input[pos] == '[', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    if (input[pos] == ']') {
        pos++;
        return {};
    }

    std::vector<scoped_ptr<Expr>> statements;
    while (true) {
        scoped_ptr<Expr> stmt = generate_expr(input, pos);
        statements.emplace_back(std::move(stmt));
        if (input[pos] == ',' && input[pos + 1] == ' ') {
            pos += 2;
            continue;
        }
        break;
    }
    array<scoped_ptr<Expr>> result = std::move(statements);
    assume(input[pos] == ']', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return result;
}


static 
array<std::string> generate_function_args(const std::string &input, size_t &pos) {
    assume(input[pos] == '[', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    if (input[pos] == ']') {
        pos++;
        return {};
    }

    std::vector<std::string> args;
    while (true) {
        assume(sstrcmp(input, "arg(arg=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
        pos += sizeof("arg(arg=") - 1;

        std::string arg = extract_delimited_substring(input, pos);
        pos += arg.size() + 2;
        args.emplace_back(std::move(arg));
        
        assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
        pos++;
        if (input[pos] == ',' && input[pos + 1] == ' ') {
            pos += 2;
            continue;
        }
        break;
    }

    array<std::string> result = std::move(args);

    assume(input[pos] == ']', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return result;
}


static
scoped_ptr<Stmt> generate_import(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "Import(names=[alias(name=", pos));

    pos += sizeof("Import(names=[alias(name=") - 1;
    std::string name = extract_delimited_substring(input, pos);
    scoped_ptr<Stmt> res = new Import{ name };
    pos += name.size() + 2;

    assume(sstrcmp(input, ")])", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(")])") - 1;
    return res;
}


static scoped_ptr<Stmt> generate_function_def(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "FunctionDef(name=", pos));
    pos += sizeof("FunctionDef(name=") - 1;
    std::string name = extract_delimited_substring(input, pos);
    pos += name.size() + 2;

    assume(sstrcmp(input, ", args=arguments(posonlyargs=[], args=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", args=arguments(posonlyargs=[], args=") - 1;

    array<std::string> args = generate_function_args(input, pos);
    assume(sstrcmp(input, ", kwonlyargs=[], kw_defaults=[], defaults=[]), body=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", kwonlyargs=[], kw_defaults=[], defaults=[]), body=") - 1;

    array<scoped_ptr<Stmt>> body = generate_stmt_array(input, pos);

    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;

    return new FunctionDef{ name, args, body };
}


static 
scoped_ptr<Stmt> generate_class_def(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "ClassDef(name=", pos));
    pos += sizeof("ClassDef(name=") - 1;

    std::string name = extract_delimited_substring(input, pos);
    pos += name.size() + 2;

    assume(sstrcmp(input, ", bases=[], keywords=[], body=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", bases=[], keywords=[], body=") - 1;
    array<scoped_ptr<Stmt>> body = generate_stmt_array(input, pos);

    assume(sstrcmp(input, ", decorator_list=[], type_params=[])", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", decorator_list=[], type_params=[])") - 1;
    return new ClassDef{ name, body };
}


static
scoped_ptr<Stmt> generate_return(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "Return(", pos));

    pos += sizeof("Return(") - 1;
    if (input[pos] == ')') {
        pos++;
        return new Return{};
    }

    assume(sstrcmp(input, "value=(", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof("value=(") - 1;
    
    scoped_ptr<Expr> value = generate_expr(input, pos);

    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return new Return{ value };
}


static 
scoped_ptr<Stmt> generate_assign(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "Assign(targets=", pos));
    pos += sizeof("Assign(targets=") - 1;

    array<scoped_ptr<Expr>> targets = generate_expr_array(input, pos);
    assume(sstrcmp(input, ", value=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", value=") - 1;

    scoped_ptr<Expr> value = generate_expr(input, pos);
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return new Assign{ targets, value };
}


static
scoped_ptr<OperatorKind> generate_operator_kind(const std::string &input, size_t &pos);


static 
scoped_ptr<Stmt>
generate_aug_assign(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "AugAssign(target=", pos));

    pos += sizeof("AugAssign(target=") - 1;
    scoped_ptr<Expr> target = generate_expr(input, pos);

    assume(sstrcmp(input, ", op=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", op=") - 1;

    scoped_ptr<BinOpKind> op = conv_or<OperatorKind, BinOpKind>(generate_operator_kind(input, pos), parse_error, pos, __FILE__, std::to_string(__LINE__));

    assume(sstrcmp(input, ", value=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", value=") - 1;

    scoped_ptr<Expr> value = generate_expr(input, pos);

    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;

    return new AugAssign{ target, op, value };
}


static 
scoped_ptr<Stmt> generate_while(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "While(test=", pos));

    pos += sizeof("While(test=") - 1;
    scoped_ptr<Expr> test = generate_expr(input, pos);

    assume(sstrcmp(input, ", body=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", body=") - 1;

    array<scoped_ptr<Stmt>> body = generate_stmt_array(input, pos);

    assume(sstrcmp(input, ", orelse=[])", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", orelse=[])") - 1;

    return new While{ test, body };
}


static
scoped_ptr<Stmt> generate_for(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "For(target=", pos));

    pos += sizeof("For(target=") - 1;
    scoped_ptr<Expr> target = generate_expr(input, pos);

    assume(sstrcmp(input, ", iter=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", iter=") - 1;

    scoped_ptr<Expr> iter = generate_expr(input, pos);

    assume(sstrcmp(input, ", body=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", body=") - 1;
    
    array<scoped_ptr<Stmt>> body = generate_stmt_array(input, pos);
    
    assume(sstrcmp(input, ", orelse=[])", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", orelse=[])") - 1;

    return new For{ target, iter, body };
}


static 
scoped_ptr<WithItem> generate_withitem(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "withitem(context_expr=", pos));
    pos += sizeof("withitem(context_expr=") - 1;

    scoped_ptr<Expr> context_expr = generate_expr(input, pos);
    if (input[pos] == ')') {
        pos++;
        return new WithItem{ context_expr };
    }
    
    assume(sstrcmp(input, ", optional_vars=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", optional_vars=") - 1;

    scoped_ptr<Expr> optional_vars = generate_expr(input, pos);
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;

    return new WithItem{ context_expr, optional_vars };
}


static
array<scoped_ptr<WithItem>> generate_withitems(const std::string &input, size_t &pos) {
    assume(input[pos] == '[', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;

    if (input[pos] == ']') {
        pos++;
        return {};
    }

    std::vector<scoped_ptr<WithItem>> items;
    while (true) {
        scoped_ptr<WithItem> item = generate_withitem(input, pos);
        items.emplace_back(std::move(item));
        if (input[pos] == ',' && input[pos + 1] == ' ') {
            pos += 2;
            continue;
        }
        break;
    }
    array<scoped_ptr<WithItem>> result = std::move(items);
    assume(input[pos] == ']', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return result;
}


static
scoped_ptr<Stmt> generate_with(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "With(items=", pos));

    pos += sizeof("With(items=") - 1;
    array<scoped_ptr<WithItem>> items = generate_withitems(input, pos);

    assume(sstrcmp(input, ", body=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", body=") - 1;

    array<scoped_ptr<Stmt>> body = generate_stmt_array(input, pos);
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;

    return new With{ items, body };
}


static 
scoped_ptr<Stmt> generate_if(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "If(test=", pos));

    pos += sizeof("If(test=") - 1;
    scoped_ptr<Expr> test = generate_expr(input, pos);

    assume(sstrcmp(input, ", body=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", body=") - 1;

    array<scoped_ptr<Stmt>> body = generate_stmt_array(input, pos);

    assume(sstrcmp(input, ", orelse=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", orelse=") - 1;

    array<scoped_ptr<Stmt>> orelse = generate_stmt_array(input, pos);
    
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return new If{ test, body, orelse };
}


static 
scoped_ptr<Stmt>
generate_expr_stmt(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "Expr(value=", pos));
    
    pos += sizeof("Expr(value=") - 1;
    scoped_ptr<Expr> expr = generate_expr(input, pos);
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return new ExprStmt{ expr };
}


static
scoped_ptr<Stmt> generate_stmt(const std::string &input, size_t &pos) {
    scoped_ptr<Stmt> res;

    if (sstrcmp(input, "Import(names=[alias(name=", pos)) {
        return generate_import(input, pos);
    }

    if (sstrcmp(input, "FunctionDef(name=", pos)) {
        return generate_function_def(input, pos);
    }

    if (sstrcmp(input, "ClassDef(name=", pos)) {
        return generate_class_def(input, pos);
    }
    
    if (sstrcmp(input, "Return(", pos)) {
        return generate_return(input, pos);
    }

    if (sstrcmp(input, "Assign(targets=", pos)) {
        return generate_assign(input, pos);
    }

    if (sstrcmp(input, "AugAssign(target=", pos)) {
        return generate_aug_assign(input, pos);
    }

    if (sstrcmp(input, "While(test=", pos)) {
        return generate_while(input, pos);
    }

    if (sstrcmp(input, "For(target=", pos)) {
        return generate_for(input, pos);
    }

    if (sstrcmp(input, "With(items=", pos)) {
        return generate_with(input, pos);
    }

    if (sstrcmp(input, "If(test=", pos)) {
        return generate_if(input, pos);
    }

    if (sstrcmp(input, "Expr(value=", pos)) {
        return generate_expr_stmt(input, pos);
    }

    if (sstrcmp(input, "Pass()", pos)) {
        pos += sizeof("Pass()") - 1;
        return new Pass{};
    }

    if (sstrcmp(input, "Break()", pos)) {
        pos += sizeof("Break()") - 1;
        return new Break{};
    }

    if (sstrcmp(input, "Continue()", pos)) {
        pos += sizeof("Continue()") - 1;
        return new Continue{};
    }

    parse_error(pos, __FILE__, std::to_string(__LINE__));
}


// currently throws runtime_error, in future need to add custom type for exceptions
static 
scoped_ptr<Module> generate_module(const std::string &input, size_t &pos) {
    assume(sstrcmp(input, "Module(", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof("Module(") - 1;
    
    assume(sstrcmp(input, "body=[", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof("body=[") - 1;
    
    if (input[pos] == ']' && input[pos + 1] == ')') { //empty module
        pos += 2;
        return new Module{ {} };
    }

    std::vector<scoped_ptr<Stmt>> statements;
    while (true) {
        scoped_ptr<Stmt> current = generate_stmt(input, pos);
        statements.emplace_back(std::move(current));
        assert(current == nullptr); // scoped_ptr implementation detail check

        if (input[pos] == ',' && input[pos + 1] == ' ') {
            pos += 2;
            continue;
        }
        break;
    }
    array<scoped_ptr<Stmt>> data = std::move(statements);

    assume(sstrcmp(input, "], type_ignores=[])", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof("], type_ignores=[])") - 1;
    return new Module{ data };
}


static
scoped_ptr<OperatorKind> generate_operator_kind(const std::string &input, size_t &pos) {
    if (sstrcmp(input, "Not()", pos)) {
        pos += sizeof("Not()") - 1;
        return new Not{};
    }
    if (sstrcmp(input, "Invert()", pos)) {
        pos += sizeof("Invert()") - 1;
        return new Invert{};
    }
    if (sstrcmp(input, "Add()", pos)) {
        pos += sizeof("Add()") - 1;
        return new Add{};
    }
    if (sstrcmp(input, "Sub()", pos)) {
        pos += sizeof("Sub()") - 1;
        return new Sub{};
    }
    if (sstrcmp(input, "Mult()", pos)) {
        pos += sizeof("Mult()") - 1;
        return new Mult{};
    }
    if (sstrcmp(input, "Div()", pos)) {
        pos += sizeof("Div()") - 1;
        return new Div{};
    }
    if (sstrcmp(input, "FloorDiv()", pos)) {
        pos += sizeof("FloorDiv()") - 1;
        return new FloorDiv{};
    }
    if (sstrcmp(input, "Mod()", pos)) {
        pos += sizeof("Mod()") - 1;
        return new Mod{};
    }
    if (sstrcmp(input, "Pow()", pos)) {
        pos += sizeof("Pow()") - 1;
        return new Pow{};
    }
    if (sstrcmp(input, "LShift()", pos)) {
        pos += sizeof("LShift()") - 1;
        return new LShift{};
    }
    if (sstrcmp(input, "RShift()", pos)) {
        pos += sizeof("RShift()") - 1;
        return new RShift{};
    }
    if (sstrcmp(input, "BitOr()", pos)) {
        pos += sizeof("BitOr()") - 1;
        return new BitOr{};
    }
    if (sstrcmp(input, "BitXor()", pos)) {
        pos += sizeof("BitXor()") - 1;
        return new BitXor{};
    }
    if (sstrcmp(input, "BitAnd()", pos)) {
        pos += sizeof("BitAnd()") - 1;
        return new BitAnd{};
    }
    if (sstrcmp(input, "And()", pos)) {
        pos += sizeof("And()") - 1;
        return new And{};
    }
    if (sstrcmp(input, "Or()", pos)) {
        pos += sizeof("Or()") - 1;
        return new Or{};
    }
    if (sstrcmp(input, "Eq()", pos)) {
        pos += sizeof("Eq()") - 1;
        return new Eq{};
    }
    if (sstrcmp(input, "NotEq()", pos)) {
        pos += sizeof("NotEq()") - 1;
        return new NotEq{};
    }
    if (sstrcmp(input, "Lt()", pos)) {
        pos += sizeof("Lt()") - 1;
        return new Lt{};
    }
    if (sstrcmp(input, "LtE()", pos)) {
        pos += sizeof("LtE()") - 1;
        return new LtE{};
    }
    if (sstrcmp(input, "Gt()", pos)) {
        pos += sizeof("Gt()") - 1;
        return new Gt{};
    }
    if (sstrcmp(input, "GtE()", pos)) {
        pos += sizeof("GtE()") - 1;
        return new GtE{};
    }
    if (sstrcmp(input, "Is()", pos)) {
        pos += sizeof("Is()") - 1;
        return new Is{};
    }
    if (sstrcmp(input, "IsNot()", pos)) {
        pos += sizeof("IsNot()") - 1;
        return new IsNot{};
    }
    if (sstrcmp(input, "In()", pos)) {
        pos += sizeof("In()") - 1;
        return new In{};
    }
    if (sstrcmp(input, "NotIn()", pos)) {
        pos += sizeof("NotIn()") - 1;
        return new NotIn{};
    }

    parse_error(pos, __FILE__, std::to_string(__LINE__));
}


static 
scoped_ptr<Expr> generate_bool_op(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "BoolOp(op=", pos));

    pos += sizeof("BoolOp(op=") - 1;
    scoped_ptr<BoolOpKind> op = conv_or<OperatorKind, BoolOpKind>(generate_operator_kind(input, pos), parse_error, pos, __FILE__, std::to_string(__LINE__));

    assume(sstrcmp(input, ", values=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", values=") - 1;

    array<scoped_ptr<Expr>> body = generate_expr_array(input, pos);
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;

    return new BoolOp{ op, body };
}


static
scoped_ptr<Expr> generate_expr(const std::string &input, size_t &pos) {
    if (sstrcmp(input, "BoolOp(op=", pos)) {
        return generate_bool_op(input, pos);
    }

    parse_error(pos, __FILE__, std::to_string(__LINE__));
}


scoped_ptr<Module> yapvm::parser::generate_ast(const std::string &input) {
    size_t pos = 0;
    scoped_ptr<Module> module = generate_module(input, pos);
    assume(pos == input.size() - 1, [] { throw std::runtime_error("AST generation error"); });
    return module;
}