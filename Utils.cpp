#include "Utils.h"
#include <fstream>
#include <sstream>

using namespace yapvm;

std::vector<std::string> yapvm::split(const std::string &str, const std::regex &re) {
    std::sregex_token_iterator iter(str.begin(), str.end(), re, -1);
    std::sregex_token_iterator end;

    return { iter, end };
}


std::string yapvm::read_file(const std::string &name) {
    std::ifstream inp(name);
    std::stringstream buffer;
    buffer << inp.rdbuf();

    return buffer.str();
}