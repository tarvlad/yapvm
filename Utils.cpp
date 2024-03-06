#include "Utils.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <cassert>


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


bool yapvm::is_number(const std::string &s) {
    return !s.empty() && s.find_first_not_of("0123456789") == std::string::npos;
}


std::vector<std::string> yapvm::matched(const std::vector<std::string> &strs, const std::regex &re) {
    std::vector<std::string> result;
    for (const std::string &s : strs) {
        if (std::regex_match(s, re)) {
            result.push_back(s);
        }
    }

    return result;
}


std::vector<std::string> yapvm::not_matched(const std::vector<std::string> &strs, const std::regex &re) {
    std::vector<std::string> result;
    for (const std::string &s : strs) {
        if (!std::regex_match(s, re)) {
            result.push_back(s);
        }
    }

    return result;
}


size_t yapvm::stosz(const std::string &str) {
    assert(!str.empty() && str.find_last_not_of("0123456789") == std::string::npos);
    std::stringstream content(str);
    size_t result;
    content >> result;
    return result;
}