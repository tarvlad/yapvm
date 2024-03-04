#pragma once

#include <string>
#include <span>
#include <vector>


namespace yapvm {

/// <summary>
///  Represents parsed bytecode instruction with it args
/// and additional info for interpreter.
/// </summary>
class InstructionP {
    std::string name_; // instruction name, one of from Opcodes.h 
    std::vector<std::string> args_;
    size_t label_; // label for jump and etc
    bool jump_target_;

public:
    InstructionP();

    InstructionP(
        const std::string &name, const std::vector<std::string> &args, 
        size_t label, bool jump_target
    );

    ~InstructionP();

    const std::string &name() const;
    
    size_t num_args() const;

    const std::string &arg(size_t i) const;

    size_t label() const;

    bool is_jump_target() const;
};


}