#pragma once

#include <string>
#include <vector>
#include "Function.h"

namespace yapvm {

std::vector<FunctionP> parse(const std::string &program_text);

}

