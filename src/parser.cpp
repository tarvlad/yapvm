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
std::vector<scoped_ptr<Stmt>> generate_stmt_vec(const std::string &input, size_t &pos) {
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
    std::vector<scoped_ptr<Stmt>> result = std::move(statements);
    assume(input[pos] == ']', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return result;
}


static
std::vector<scoped_ptr<Expr>> generate_expr_vec(const std::string &input, size_t &pos) {
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
    std::vector<scoped_ptr<Expr>> result = std::move(statements);
    assume(input[pos] == ']', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return result;
}


static 
std::vector<std::string> generate_function_args(const std::string &input, size_t &pos) {
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

    std::vector<std::string> result = std::move(args);

    assume(input[pos] == ']', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return result;
}


static
scoped_ptr<Stmt> generate_import(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "Import(names=[alias(name=", pos));

    pos += sizeof("Import(names=[alias(name=") - 1;
    std::string name = extract_delimited_substring(input, pos);
    pos += name.size() + 2;
    scoped_ptr<Stmt> res = new Import{ std::move(name) };

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

    std::vector<std::string> args = generate_function_args(input, pos);
    assume(sstrcmp(input, ", kwonlyargs=[], kw_defaults=[], defaults=[]), body=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", kwonlyargs=[], kw_defaults=[], defaults=[]), body=") - 1;

    std::vector<scoped_ptr<Stmt>> body = generate_stmt_vec(input, pos);

    assume(sstrcmp(input, ", decorator_list=[], type_params=[])", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", decorator_list=[], type_params=[])") - 1;

    return new FunctionDef{ std::move(name), std::move(args), std::move(body) };
}


static 
scoped_ptr<Stmt> generate_class_def(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "ClassDef(name=", pos));
    pos += sizeof("ClassDef(name=") - 1;

    std::string name = extract_delimited_substring(input, pos);
    pos += name.size() + 2;

    assume(sstrcmp(input, ", bases=[], keywords=[], body=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", bases=[], keywords=[], body=") - 1;
    std::vector<scoped_ptr<Stmt>> body = generate_stmt_vec(input, pos);

    assume(sstrcmp(input, ", decorator_list=[], type_params=[])", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", decorator_list=[], type_params=[])") - 1;
    return new ClassDef{ std::move(name), std::move(body) };
}


static
scoped_ptr<Stmt> generate_return(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "Return(", pos));

    pos += sizeof("Return(") - 1;
    if (input[pos] == ')') {
        pos++;
        return new Return{};
    }

    assume(sstrcmp(input, "value=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof("value=") - 1;
    
    scoped_ptr<Expr> value = generate_expr(input, pos);

    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return new Return{ std::move(value) };
}


static 
scoped_ptr<Stmt> generate_assign(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "Assign(targets=", pos));
    pos += sizeof("Assign(targets=") - 1;

    std::vector<scoped_ptr<Expr>> targets = generate_expr_vec(input, pos);
    assume(sstrcmp(input, ", value=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", value=") - 1;

    scoped_ptr<Expr> value = generate_expr(input, pos);
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return new Assign{ std::move(targets), std::move(value) };
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

    return new AugAssign{ std::move(target), std::move(op), std::move(value) };
}


static 
scoped_ptr<Stmt> generate_while(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "While(test=", pos));

    pos += sizeof("While(test=") - 1;
    scoped_ptr<Expr> test = generate_expr(input, pos);

    assume(sstrcmp(input, ", body=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", body=") - 1;

    std::vector<scoped_ptr<Stmt>> body = generate_stmt_vec(input, pos);

    assume(sstrcmp(input, ", orelse=[])", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", orelse=[])") - 1;

    return new While{ std::move(test), std::move(body) };
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
    
    std::vector<scoped_ptr<Stmt>> body = generate_stmt_vec(input, pos);
    
    assume(sstrcmp(input, ", orelse=[])", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", orelse=[])") - 1;

    return new For{ std::move(target), std::move(iter), std::move(body) };
}


static 
scoped_ptr<WithItem> generate_withitem(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "withitem(context_expr=", pos));
    pos += sizeof("withitem(context_expr=") - 1;

    scoped_ptr<Expr> context_expr = generate_expr(input, pos);
    if (input[pos] == ')') {
        pos++;
        return new WithItem{ std::move(context_expr) };
    }
    
    assume(sstrcmp(input, ", optional_vars=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", optional_vars=") - 1;

    scoped_ptr<Expr> optional_vars = generate_expr(input, pos);
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;

    return new WithItem{ std::move(context_expr), std::move(optional_vars) };
}


static
std::vector<scoped_ptr<WithItem>> generate_withitems(const std::string &input, size_t &pos) {
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
    std::vector<scoped_ptr<WithItem>> result = std::move(items);
    assume(input[pos] == ']', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return result;
}


static
scoped_ptr<Stmt> generate_with(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "With(items=", pos));

    pos += sizeof("With(items=") - 1;
    std::vector<scoped_ptr<WithItem>> items = generate_withitems(input, pos);

    assume(sstrcmp(input, ", body=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", body=") - 1;

    std::vector<scoped_ptr<Stmt>> body = generate_stmt_vec(input, pos);
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;

    return new With{ std::move(items), std::move(body) };
}


static 
scoped_ptr<Stmt> generate_if(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "If(test=", pos));

    pos += sizeof("If(test=") - 1;
    scoped_ptr<Expr> test = generate_expr(input, pos);

    assume(sstrcmp(input, ", body=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", body=") - 1;

    std::vector<scoped_ptr<Stmt>> body = generate_stmt_vec(input, pos);

    assume(sstrcmp(input, ", orelse=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", orelse=") - 1;

    std::vector<scoped_ptr<Stmt>> orelse = generate_stmt_vec(input, pos);
    
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return new If{ std::move(test), std::move(body), std::move(orelse) };
}


static 
scoped_ptr<Stmt>
generate_expr_stmt(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "Expr(value=", pos));
    
    pos += sizeof("Expr(value=") - 1;
    scoped_ptr<Expr> expr = generate_expr(input, pos);
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return new ExprStmt{ std::move(expr) };
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
    assert(false);
}


// currently throws runtime_error, in future need to add custom type for exceptions
static 
scoped_ptr<Module> generate_module(const std::string &input, size_t &pos) {
    assume(sstrcmp(input, "Module(", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof("Module(") - 1;
    
    assume(sstrcmp(input, "body=[", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof("body=[") - 1;
    
    if (input[pos] == ']' && sstrcmp(input, ", type_ignores=[])", pos + 1)) { //empty module
        pos += sizeof(", type_ignores=[])") - 1 + 1;
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
    std::vector<scoped_ptr<Stmt>> data = std::move(statements);

    assume(sstrcmp(input, "], type_ignores=[])", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof("], type_ignores=[])") - 1;
    return new Module{ std::move(data) };
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
    if (sstrcmp(input, "USub()", pos)) {
        pos += sizeof("USub()") - 1;
        return new USub{};
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
    assert(false);
}


static 
scoped_ptr<Expr> generate_bool_op(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "BoolOp(op=", pos));

    pos += sizeof("BoolOp(op=") - 1;
    scoped_ptr<BoolOpKind> op = conv_or<OperatorKind, BoolOpKind>(generate_operator_kind(input, pos), parse_error, pos, __FILE__, std::to_string(__LINE__));

    assume(sstrcmp(input, ", values=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", values=") - 1;

    std::vector<scoped_ptr<Expr>> body = generate_expr_vec(input, pos);
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;

    return new BoolOp{ std::move(op), std::move(body) };
}


static
scoped_ptr<Expr> generate_bin_op(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "BinOp(left=", pos));

    pos += sizeof("BinOp(left=") - 1;
    scoped_ptr<Expr> left = generate_expr(input, pos);

    assume(sstrcmp(input, ", op=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", op=") - 1;
    scoped_ptr<BinOpKind> op = conv_or<OperatorKind, BinOpKind>(generate_operator_kind(input, pos), parse_error, pos, __FILE__, std::to_string(__LINE__));

    assume(sstrcmp(input, ", right=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", right=") - 1;

    scoped_ptr<Expr> right = generate_expr(input, pos);
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return new BinOp{ std::move(left), std::move(op), std::move(right) };
}


static
scoped_ptr<Expr> generate_unary_op(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "UnaryOp(op=", pos));

    pos += sizeof("UnaryOp(op=") - 1;
    scoped_ptr<UnaryOpKind> op = conv_or<OperatorKind, UnaryOpKind>(generate_operator_kind(input, pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    
    assume(sstrcmp(input, ", operand=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", operand=") - 1;

    scoped_ptr<Expr> operand = generate_expr(input, pos);

    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return new UnaryOp{ std::move(op), std::move(operand) };
}


static 
std::vector<scoped_ptr<CmpOpKind>> generate_cmp_op_vec(const std::string &input, size_t &pos) {
    assume(input[pos] == '[', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;

    if (input[pos] == ']') {
        pos++;
        return {};
    }

    std::vector<scoped_ptr<CmpOpKind>> ops;
    while (true) {
        scoped_ptr<CmpOpKind> op = conv_or<OperatorKind, CmpOpKind>(generate_operator_kind(input, pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
        ops.emplace_back(std::move(op));
        if (input[pos] == ',' && input[pos + 1] == ' ') {
            pos += 2;
            continue;
        }
        break;
    }
    
    std::vector<scoped_ptr<CmpOpKind>> res = std::move(ops);
    assume(input[pos] == ']', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return res;
}


static
scoped_ptr<Expr> generate_compare(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "Compare(left=", pos));

    pos += sizeof("Compare(left=") - 1;
    
    scoped_ptr<Expr> left = generate_expr(input, pos);
    assume(sstrcmp(input, ", ops=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", ops=") - 1;

    std::vector<scoped_ptr<CmpOpKind>> ops = generate_cmp_op_vec(input, pos);

    assume(sstrcmp(input, ", comparators=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", comparators=") - 1;

    std::vector<scoped_ptr<Expr>> comparators = generate_expr_vec(input, pos);

    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return new Compare{ std::move(left), std::move(ops), std::move(comparators) };
}


static
scoped_ptr<Expr> generate_call(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "Call(func=", pos));

    pos += sizeof("Call(func=") - 1;
    scoped_ptr<Expr> func = generate_expr(input, pos);
    
    assume(sstrcmp(input, ", args=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", args=") - 1;

    std::vector<scoped_ptr<Expr>> args = generate_expr_vec(input, pos);

    assume(sstrcmp(input, ", keywords=[])", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", keywords=[])") - 1;
    return new Call{ std::move(func), std::move(args) };
}


size_t try_tokenize_int(const std::string &str, size_t cursor_pos) {
    size_t int_len = 0;
    if (str[cursor_pos] == '-') {
        int_len++;
        cursor_pos++;
    }
    if (cursor_pos == str.size()) {
        return 0;
    }
    while (isdigit(str[cursor_pos])) {
        int_len++;
        cursor_pos++;
        if (cursor_pos == str.size()) {
            break;
        }
    }

    return int_len;
}


size_t try_tokenize_float(const std::string &str, size_t cursor_pos) {
    size_t float_first_part_len = try_tokenize_int(str, cursor_pos);
    if (float_first_part_len == 0) {
        return 0;
    }
    cursor_pos += float_first_part_len;

    if (str[cursor_pos] != '.') {
        return 0;
    }
    cursor_pos++;
    size_t float_second_part_len = try_tokenize_int(str, cursor_pos);
    return float_first_part_len + float_second_part_len + 1;
}



static 
scoped_ptr<YPrimitiveObject> generate_constant_value(const std::string &input, size_t &pos) {
    if (input[pos] == '\'') {
        std::string val = extract_delimited_substring(input, pos);
        pos += val.size() + 2;
        scoped_ptr<YPrimitiveObject> obj = new YStringObject{ val };
        return obj;
    }

    size_t possible_float_len = try_tokenize_float(input, pos);
    if (possible_float_len != 0) {
        std::string s_val = input.substr(pos, possible_float_len);
        pos += possible_float_len;
        double val = from_str<double>(s_val);
        scoped_ptr<YPrimitiveObject> obj = new YFloatObject{ val };
        return obj;
    }

    size_t possible_int_len = try_tokenize_int(input, pos);
    if (possible_int_len != 0) {
        std::string s_val = input.substr(pos, possible_int_len);
        pos += possible_int_len;
        ssize_t val = from_str<ssize_t>(s_val);
        scoped_ptr<YPrimitiveObject> obj = new YIntObject{ val };
        return obj;
    }

    parse_error(pos, __FILE__, std::to_string(__LINE__));
    assert(false);
}


static
scoped_ptr<Expr> generate_constant(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "Constant(value=", pos));

    pos += sizeof("Constant(value=") - 1;
    scoped_ptr<YPrimitiveObject> value = generate_constant_value(input, pos);
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;

    return new Constant{ std::move(value) };
}


static
scoped_ptr<ExprContext> generate_expr_context(const std::string &input, size_t &pos) {
    if (sstrcmp(input, "Load()", pos)) {
        pos += sizeof("Load()") - 1;
        return new Load{};
    }
    if (sstrcmp(input, "Store()", pos)) {
        pos += sizeof("Store()") - 1;
        return new Store{};
    }
    if (sstrcmp(input, "Del()", pos)) {
        pos += sizeof("Del()") - 1;
        return new Del{};
    }

    parse_error(pos, __FILE__, std::to_string(__LINE__));
    assert(false);
}


static
scoped_ptr<Expr> generate_attribute(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "Attribute(value=", pos));

    pos += sizeof("Attribute(value=") - 1;
    scoped_ptr<Expr> value = generate_expr(input, pos);

    assume(sstrcmp(input, ", attr=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", attr=") - 1;
    std::string attr = extract_delimited_substring(input, pos);
    pos += attr.size() + 2;
    
    assume(sstrcmp(input, ", ctx=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", ctx=") - 1;

    scoped_ptr<ExprContext> ctx = generate_expr_context(input, pos);
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return new Attribute{ std::move(value), std::move(attr), std::move(ctx) };
}


static
scoped_ptr<Expr> generate_subscript(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "Subscript(value=", pos));

    pos += sizeof("Subscript(value=") - 1;
    scoped_ptr<Expr> value = generate_expr(input, pos);

    assume(sstrcmp(input, ", slice=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", slice=") - 1;

    scoped_ptr<Expr> slice = generate_expr(input, pos);
    assume(sstrcmp(input, ", ctx=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", ctx=") - 1;

    scoped_ptr<ExprContext> ctx = generate_expr_context(input, pos);
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return new Subscript{ std::move(value), std::move(slice), std::move(ctx) };
}


static 
scoped_ptr<Expr> generate_name(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "Name(id=", pos));

    pos += sizeof("Name(id=") - 1;
    std::string name = extract_delimited_substring(input, pos);
    pos += name.size() + 2;

    assume(sstrcmp(input, ", ctx=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", ctx=") - 1;

    scoped_ptr<ExprContext> ctx = generate_expr_context(input, pos);

    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return new Name{ std::move(name), std::move(ctx) };
}


static
scoped_ptr<Expr> generate_expr(const std::string &input, size_t &pos) {
    if (sstrcmp(input, "BoolOp(op=", pos)) {
        return generate_bool_op(input, pos);
    }
    if (sstrcmp(input, "BinOp(left=", pos)) {
        return generate_bin_op(input, pos);
    }
    if (sstrcmp(input, "UnaryOp(op=", pos)) {
        return generate_unary_op(input, pos);
    }
    if (sstrcmp(input, "Compare(left=", pos)) {
        return generate_compare(input, pos);
    }
    if (sstrcmp(input, "Call(func=", pos)) {
        return generate_call(input, pos);
    }
    if (sstrcmp(input, "Constant(value=", pos)) {
        return generate_constant(input, pos);
    }
    if (sstrcmp(input, "Attribute(value=", pos)) {
        return generate_attribute(input, pos);
    }
    if (sstrcmp(input, "Subscript(value=", pos)) {
        return generate_subscript(input, pos);
    }
    if (sstrcmp(input, "Name(id=", pos)) {
        return generate_name(input, pos);
    }
    parse_error(pos, __FILE__, std::to_string(__LINE__));
    assert(false);
}


scoped_ptr<Module> yapvm::parser::generate_ast(const std::string &input) {
    size_t pos = 0;
    scoped_ptr<Module> module = generate_module(input, pos);
    assume(pos == input.size(), [] { throw std::runtime_error("AST generation error"); });
    return module;
}