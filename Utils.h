#pragma once

#include <vector>
#include <string>
#include <regex>


namespace yapvm {

std::vector<std::string> split(const std::string &str, const std::regex &re = std::regex{ "\\s+" });

std::string read_file(const std::string &name);

}