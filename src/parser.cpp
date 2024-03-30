#include "parser.h"
#include "utils.h"
#include "ast.h"
#include <stdexcept>
#include <string>


using namespace yapvm::parser;
using namespace yapvm::ast;
using namespace yapvm;


static scoped_ptr<Import> generate_import(const std::string &input, size_t &pos);//TODO


static scoped_ptr<Stmt> generate_stmt(const std::string &input, size_t &pos) {
    auto parse_error = [pos] -> void {
        throw std::runtime_error("Error while module generation at pos [" + std::to_string(pos) + "]");
    };    

    scoped_ptr<Stmt> res;
    check(sstrcmp(input, "Import(names=[alias(name=", pos))
    .and_then(
        [&res, &pos, &input, &parse_error] -> void {
            pos += sizeof("Import(names=[alias(name=") - 1;
            std::string name = extract_delimited_substring(input, pos);
            res = new Import{ name };
            pos += name.size() + 2;
            
            check(sstrcmp(input, ")])", pos))
            .and_then(
                [&pos] -> void {
                    pos += sizeof(")])") - 1;
                }
            )
            .or_else(
                parse_error
            );
        }
    );
    RETURN_IF(res != nullptr, res);

    

    //TODO
    return nullptr;
}


// currently throws runtime_error, in future need to add custom type for exceptions
static 
scoped_ptr<Module> generate_module(const std::string &input, size_t &pos) {
    auto parse_error = [pos] -> void {
        throw std::runtime_error("Error while module generation at pos [" + std::to_string(pos) + "]");
    };

    check(sstrcmp(input, "Module(", pos))
        .and_then([&pos] -> void { pos += sizeof("Module(") - 1; })
        .or_else(parse_error);
    
    check(sstrcmp(input, "body=[", pos))
        .and_then([&pos] -> void { pos += sizeof("body=[") - 1; })
        .or_else(parse_error);


    //TODO statements

    check(input[pos] == ']').and_then([&pos] -> void { pos++; }).or_else(parse_error);
    check(input[pos] == ')').or_else(parse_error);
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