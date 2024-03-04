#include "Parser.h"
#include "Instruction.h"
#include "Function.h"
#include <inttypes.h>


using namespace yapvm;


InstructionP::InstructionP(
    const std::string &name, 
    const std::vector<std::string> &args,  
    size_t label, 
    bool jump_target
) 
    : name_{ name }, args_{ new std::string[args.size()], args.size()},
    label_{ label }, jump_target_{ jump_target } {

    for (size_t i = 0; i < args.size(); i++) {
        args_[i] = args[i];
    }
}


InstructionP::~InstructionP() {
    delete[] args_.data();
}


const std::string &InstructionP::name() const {
    return name_;
}


size_t InstructionP::num_args() const {
    return args_.size();
}


const std::string &InstructionP::arg(size_t i) const {
    return args_[i];
}


size_t InstructionP::label() const {
    return label_;
}


bool InstructionP::is_jump_target() const {
    return jump_target_;
}


InstructionP::InstructionP()
    : name_{ }, args_{ }, label_{ }, jump_target_{ } {}


FunctionP::FunctionP(std::string name, std::vector<InstructionP> code) 
    : name_{ name }, 
    code_{ new InstructionP[code.size()], code.size()} {

    for (size_t i = 0; i < code.size(); i++) {
        code_[i] = code[i];
    }
}


FunctionP::~FunctionP() {
    delete[] code_.data();
}


const std::string &FunctionP::name() const {
    return name_;
}


size_t FunctionP::num_instrs() const {
    return code_.size();
}


const InstructionP &FunctionP::instr(size_t idx) const {
    return code_[idx];
}