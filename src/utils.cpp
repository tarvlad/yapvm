#include "utils.h"
#include <sstream>
#include <fstream>
#include <cassert>

using namespace yapvm;


std::string yapvm::read_file(const std::string &s) {
    std::ifstream t{ s };
    assert(t.is_open());

    std::stringstream buffer;

    buffer << t.rdbuf();
    return buffer.str();
}