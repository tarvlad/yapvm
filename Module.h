#pragma once

#include "Function.h"
#include <vector>
#include <string>


namespace yapvm {


class Module {
    std::string name_;
    std::vector<FunctionP> units_;

public:
    Module(const std::string &name, const std::vector<FunctionP> &units);

    const std::string &name() const;

    size_t num_units() const;

    const FunctionP &unit(size_t idx) const;
};


}