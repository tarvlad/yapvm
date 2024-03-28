#include "parser.h"
#include "utils.h"
#include "ast.h"
#include <stdexcept>


using namespace yapvm::parser;
using namespace yapvm::ast;
using namespace yapvm;


// currently throws runtime_error, in future need to add custom type for exceptions
static 
scoped_ptr<Module> generate_module(const std::string_view &input, size_t &pos) {
    if (input.substr(pos, pos + 7) != "Module(") {
        throw std::runtime_error("Expected Module(");
    }
    pos += 7;
    if (input.substr(pos, pos + 6) != "body=[") {
        throw std::runtime_error("Expected body=[ in begin of module definition");
    }
    pos += 6;
    
    if (input.substr(pos, pos + 7) == "Import(") {
        
    }

    if (input[pos] != ']') {
        throw std::runtime_error("Expected end of module body");
    }
    pos++;
    if (input[pos] != ')') {
        throw std::runtime_error("Expected end of module definition");
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