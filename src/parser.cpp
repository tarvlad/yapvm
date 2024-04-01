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

    scoped_ptr<OperatorKind> op = generate_operator_kind(input, pos);

    assume(sstrcmp(input, ", value=", pos), parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos += sizeof(", value=") - 1;

    scoped_ptr<Expr> value = generate_expr(input, pos);

    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;

    return new AugAssign{ target, *op, value };
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
    //TODO
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

    assume(input[pos] == ']', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++; 
    assume(input[pos] == ')', parse_error, pos, __FILE__, std::to_string(__LINE__));
    pos++;
    return new Module{ data };
}


static
scoped_ptr<OperatorKind> generate_operator_kind(const std::string &input, size_t &pos) {
    throw std::runtime_error("Not implemented currently"); //TODO
}


static
scoped_ptr<Expr> generate_expr(const std::string &input, size_t &pos) {
    throw std::runtime_error("Not implemented currently"); //TODO
}


scoped_ptr<Module> yapvm::parser::generate_ast(const std::string &input) {
    size_t pos = 0;
    scoped_ptr<Module> module = generate_module(input, pos);
    assume(pos == input.size() - 1, [] { throw std::runtime_error("AST generation error"); });
    return module;
}