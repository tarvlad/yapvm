#pragma once

#include <vector>
#include <string>
#include <regex>


namespace yapvm {

std::vector<std::string> split(const std::string &str, const std::regex &re = std::regex{ "\\s+" });

std::string read_file(const std::string &name);

bool is_number(const std::string &s);

std::vector<std::string> matched(const std::vector<std::string> &strs, const std::regex &re);

std::vector<std::string> not_matched(const std::vector<std::string> &strs, const std::regex &re);

size_t stosz(const std::string &str);

}