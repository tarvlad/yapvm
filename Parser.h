#pragma once

#include <string>
#include <vector>
#include "Function.h"
#include "Module.h"

namespace yapvm {

Module parse(const std::string &module_name, const std::string &program_text);

}

