#pragma once

#include "Instruction.h"
#include <vector>
#include <string>

namespace yapvm {

/// <summary>
/// Represents parsed function.
/// </summary>
class FunctionP {
    std::string name_;
    std::span<InstructionP> code_;

public:
    FunctionP(
        std::string name,
        std::vector<InstructionP> code
    );

    ~FunctionP();

    const std::string &name() const;

    size_t num_instrs() const;

    const InstructionP &instr(size_t idx) const;
};


}