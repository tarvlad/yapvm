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
array<std::string> generate_function_args(const std::string &input, size_t &pos) {
    if (input[pos] != '[') {
        parse_error(pos, __FILE__, std::to_string(__LINE__));
    }
    pos++;
    if (input[pos] == ']') {
        pos++;
        return {};
    }

    std::vector<std::string> args;
    while (true) {
        if (!sstrcmp(input, "arg(arg=", pos)) {
            parse_error(pos, __FILE__, std::to_string(__LINE__));
        }
        pos += sizeof("arg(arg=") - 1;
        std::string arg = extract_delimited_substring(input, pos);
        pos += arg.size() + 2;
        args.emplace_back(std::move(arg));
        if (input[pos] != ')') {
            parse_error(pos, __FILE__, std::to_string(__LINE__));
        }
        pos++;
        if (input[pos] == ',' && input[pos + 1] == ' ') {
            pos += 2;
            continue;
        }
        break;
    }

    array<std::string> result = std::move(args);

    if (input[pos] != ']') {
        parse_error(pos, __FILE__, std::to_string(__LINE__));
    }
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

    if (!sstrcmp(input, ")])", pos)) {
        parse_error(pos, __FILE__, std::to_string(__LINE__));
    }
    pos += sizeof(")])") - 1;
    return res;
}


static
scoped_ptr<Stmt> generate_stmt(const std::string &input, size_t &pos);


static
array<scoped_ptr<Stmt>> generate_function_body(const std::string &input, size_t &pos) {
    if (input[pos] != '[') {
        parse_error(pos, __FILE__, std::to_string(__LINE__));
    }
    pos++;
    if (input[pos] == ']') {
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
    if (input[pos] != ']') {
        parse_error(pos, __FILE__, std::to_string(__LINE__));
    }
    pos++;
    return result;
}


static scoped_ptr<Stmt> generate_function_def(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "FunctionDef(name=", pos));
    pos += sizeof("FunctionDef(name=") - 1;
    std::string name = extract_delimited_substring(input, pos);
    pos += name.size() + 2;
    if (!sstrcmp(input, ", args=arguments(posonlyargs=[], args=", pos)) {
        parse_error(pos, __FILE__, std::to_string(__LINE__));
    }
    pos += sizeof(", args=arguments(posonlyargs=[], args=") - 1;

    array<std::string> args = generate_function_args(input, pos);
    if (!sstrcmp(input, ", kwonlyargs=[], kw_defaults=[], defaults=[]), body=", pos)) {
        parse_error(pos, __FILE__, std::to_string(__LINE__));
    }
    pos += sizeof(", kwonlyargs=[], kw_defaults=[], defaults=[]), body=") - 1;

    array<scoped_ptr<Stmt>> statements = generate_function_body(input, pos);

    if (input[pos] != ')') {
        parse_error(pos, __FILE__, std::to_string(__LINE__));
    }
    pos++;

    return new FunctionDef{ name, args, statements };
}


static 
scoped_ptr<Stmt> generate_class_def(const std::string &input, size_t &pos) {
    assert(sstrcmp(input, "ClassDef(name=", pos));

    std::string name = extract_delimited_substring(input, pos);
    pos += name.size() + 2;

    if (!sstrcmp(input, ", bases=[], keywords=[], body=", pos)) {
        parse_error(pos, __FILE__, std::to_string(__LINE__));
    }
    pos += sizeof(", bases=[], keywords=[], body=") - 1;
    //TODO
    return nullptr;
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
    

    //TODO
    parse_error(pos, __FILE__, std::to_string(__LINE__));
}


// currently throws runtime_error, in future need to add custom type for exceptions
static 
scoped_ptr<Module> generate_module(const std::string &input, size_t &pos) {
    if (!sstrcmp(input, "Module(", pos)) {
        parse_error(pos, __FILE__, std::to_string(__LINE__));
    }
    pos += sizeof("Module(") - 1;
    
    if (!sstrcmp(input, "body=[", pos)) {
        parse_error(pos, __FILE__, std::to_string(__LINE__));
    }
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

    if (input[pos] != ']') {
        parse_error(pos, __FILE__, std::to_string(__LINE__));
    }
    pos++;
    if (input[pos] != ')') {
        parse_error(pos, __FILE__, std::to_string(__LINE__));
    } 
    pos++;
    return new Module{ data };
}


scoped_ptr<Module> yapvm::parser::generate_ast(const std::string &input) {
    size_t pos = 0;
    scoped_ptr<Module> module = generate_module(input, pos);
    if (pos != input.size() - 1) {
        throw std::runtime_error("AST generation error");
    }
    return module;
}