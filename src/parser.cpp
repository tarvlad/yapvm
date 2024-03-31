#include "parser.h"
#include "utils.h"
#include "ast.h"
#include <stdexcept>
#include <string>


using namespace yapvm::parser;
using namespace yapvm::ast;
using namespace yapvm;


static void parse_error(size_t pos) {
    throw std::runtime_error("Error while module generation at pos [" + std::to_string(pos) + "]");
}


static scoped_ptr<Import> generate_import(const std::string &input, size_t &pos);//TODO


static scoped_ptr<Stmt> generate_stmt(const std::string &input, size_t &pos) {
    scoped_ptr<Stmt> res;

    if (sstrcmp(input, "Import(names=[alias(name=", pos)) {
        pos += sizeof("Import(names=[alias(name=") - 1;
        std::string name = extract_delimited_substring(input, pos);
        res = new Import{ name };
        pos += name.size() + 2;

        if (!sstrcmp(input, ")])", pos)) {
            parse_error(pos);
        }
        pos += sizeof(")])") - 1;
        return res;
    }


    

    //TODO
    return nullptr;
}


// currently throws runtime_error, in future need to add custom type for exceptions
static 
scoped_ptr<Module> generate_module(const std::string &input, size_t &pos) {
    if (!sstrcmp(input, "Module(", pos)) {
        parse_error(pos);
    }
    pos += sizeof("Module(") - 1;
    
    if (!sstrcmp(input, "body=[", pos)) {
        parse_error(pos);
    }
    pos += sizeof("body=[") - 1;

    //TODO generate stmt-s

    if (input[pos] != ']') {
        parse_error(pos);
    }
    pos++;
    if (input[pos] != ')') {
        parse_error(pos);
    }
    //TODO return 
    return nullptr; //tmp
}


scoped_ptr<Module> yapvm::parser::generate_ast(const std::string &input) {
    size_t pos = 0;
    scoped_ptr<Module> module = generate_module(input, pos);
    if (pos != input.size() - 1) {
        throw std::runtime_error("AST generation error");
    }
    return module;
}