#include "lexer.h"
#include <print>
#include <regex>
#include "lexer.h"
#include "utils.h"

using namespace yapvm;


int main() {
    std::string test_source = read_file("../test_resources/lexer_test_all.py");
    std::vector<Token> tokens = tokenize(test_source);
    int x = 42;
}