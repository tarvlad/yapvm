#include "parser.h"
#include "utils.h"


using namespace yapvm::parser;
using namespace yapvm::ast;
using namespace yapvm;


static 
scoped_ptr<Node> ast_gen(const std::string_view &input, size_t &pos) {
    if (is_there_at_least_n_elements_in<7>(input, pos)) {
        if (input.substr(pos, pos + 7) == "Module(") {
            pos += 7;
            //TODO body= ...
        }
    }
}


std::vector<Node *> generate_ast(const std::string &input) {

}