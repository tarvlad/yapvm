#include "Parser.h"
#include "Instruction.h"
#include "Function.h"
#include "Utils.h"
#include "Module.h"
#include <utility>
#include <inttypes.h>
#include <cassert>

using namespace yapvm;


InstructionP::InstructionP(
    const std::string &name, 
    const std::vector<std::string> &args,  
    size_t label, 
    bool jump_target
) 
    : name_{ name }, args_{ args },
    label_{ label }, jump_target_{ jump_target } {
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


FunctionP::FunctionP(const std::string &name, const std::vector<InstructionP> &code) 
    : name_{ name }, 
    code_{ code } {
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


static bool is_code_object_line(const std::string &line) {
    return line.substr(0, 15) == "Disassembly of " && line[line.size() - 1] == ':';
}


static std::vector<InstructionP> parse_code(std::span<std::string> code_lines) {
    return {};//TODO
}


static std::string parse_code_object_name(const std::string &name) {
    assert(is_code_object_line(name));
    return "";//TODO
}


Module yapvm::parse(const std::string &module_name, const std::string &program_text) {
    std::vector<std::string> lines = split(program_text, std::regex("\n"));

    std::vector<size_t> function_def_lines;
    for (size_t i = 0; i < lines.size(); i++) {
        if (is_code_object_line(lines[i])) {
            function_def_lines.push_back(i);
        }
    }

    size_t main_code_end = lines.size();
    if (function_def_lines.size() != 0) {
        main_code_end = function_def_lines[0];
    }
    
    std::span<std::string> main_code = std::span{ lines }.subspan(1, main_code_end - 1);

    std::vector<std::span<std::string>> functions_code;
    functions_code.push_back(main_code);

    for (size_t i = 1; i < std::min(function_def_lines.size() - 1, (size_t)1); i++) {
        size_t code_beg = function_def_lines[i] + 1;
        size_t code_end = function_def_lines[i + 1];
        functions_code.emplace_back(
            std::span{ lines }.subspan(code_beg, code_end - code_beg - 1)
        );
    }

    if (function_def_lines.size() != 0) {
        functions_code.emplace_back(
            std::span{ lines }.subspan(
                function_def_lines[function_def_lines.size() - 1] + 1, 
                lines.size() - function_def_lines[function_def_lines.size() - 1] - 1
            )
        );
    }

    std::vector<std::string> function_names;
    function_names.emplace_back("__yapvm_entry__");
    for (size_t def_line : function_def_lines) {
        function_names.emplace_back(parse_code_object_name(lines[def_line]));
    }

    for (size_t i = 1; i < function_names.size(); i++) {
        assert(function_names[i] != "__yapvm_entry__");
    }

    std::vector<FunctionP> functions;
    for (size_t i = 0; i < function_names.size(); i++) {
        std::vector<InstructionP> instructions = parse_code(functions_code[i]);
        functions.emplace_back(function_names[i], instructions);
    }

    return { module_name, functions };
}


Module::Module(const std::string &name, const std::vector<FunctionP> &units)
    : name_{ name }, units_{ units } {}


const std::string &Module::name() const {
    return name_;
}


size_t Module::num_units() const {
    return units_.size();
}


const FunctionP &Module::unit(size_t idx) const {
    return units_[idx];
}