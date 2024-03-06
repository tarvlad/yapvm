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
    std::string args_;
    size_t arg_id_;
    size_t label_; // label for jump and etc
    bool jump_target_;

public:
    InstructionP();

    InstructionP(
        const std::string &name, const std::string &args, 
        size_t arg_id_,
        size_t label, bool jump_target
    );

    const std::string &name() const;

    const std::string &args() const;

    size_t label() const;

    bool is_jump_target() const;
};


}