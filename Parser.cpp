#include "Parser.h"
#include "Instruction.h"
#include "Function.h"
#include "Utils.h"
#include "Module.h"
#include <utility>
#include <inttypes.h>
#include <cassert>
#include <optional>
#include <set>
#include <utility>

using namespace yapvm;


InstructionP::InstructionP(
    const std::string &name, 
    const std::string &args,  
    size_t arg_id,
    size_t label, 
    bool jump_target
) 
    : name_{ name }, args_{ args }, arg_id_{ arg_id },
    label_{ label }, jump_target_{ jump_target } {
}


const std::string &InstructionP::name() const {
    return name_;
}


const std::string &InstructionP::args() const {
    return args_;
}


size_t InstructionP::label() const {
    return label_;
}


bool InstructionP::is_jump_target() const {
    return jump_target_;
}


InstructionP::InstructionP()
    : name_{ }, args_{ }, label_{ }, arg_id_{ }, jump_target_ { } {}


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
    std::vector<std::string> tokens = split(line);
    return tokens[0] == "Disassembly" && tokens[1] == "of" && line.contains('<') && line.contains('>');
}


static std::set<std::string> ACCEPTED_INSTRUCTIONS = {
    //TODO
};


static std::pair<std::string, std::string> wrap_instr_args(const std::string &instr) {
    size_t first_brace_pos = instr.find_first_of('(');
    size_t last_brace_pos = instr.find_last_of(')');

    std::string instr_code = instr.substr(0, first_brace_pos);
    std::string instr_args = instr.substr(first_brace_pos, last_brace_pos - first_brace_pos + 1);

    return { instr_code, instr_args };
}


static InstructionP parse_instruction(const std::string &line) {
    //TODO get additional content in (...), after this: tokenize, parse and return
    assert(!line.empty());

    std::pair<std::string, std::string> divided_instr = wrap_instr_args(line);
    std::string &instr_code = divided_instr.first;
    std::string &args = divided_instr.second;

    std::vector<std::string> instr_code_tokens = not_matched(split(instr_code), std::regex{ "\\s*" });
    assert(instr_code_tokens.size() >= 2);

    size_t line_num_offset = 0;
    if (is_number(instr_code_tokens[0]) && is_number(instr_code_tokens[1])) {
        line_num_offset = 1;
    }

    size_t label = stosz(instr_code_tokens[line_num_offset]);
    std::string &name = instr_code_tokens[1 + line_num_offset];
    size_t arg_id = stosz(instr_code_tokens[2 + line_num_offset]);

    //assert(ACCEPTED_INSTRUCTIONS.contains(name));
    return {};//TODO
}


static std::vector<InstructionP> parse_code(std::span<std::string> code_lines) {
    std::vector<InstructionP> instructions;
    for (const std::string &line : code_lines) {
        instructions.emplace_back(parse_instruction(line));
    }
    return instructions;
}


static std::string parse_code_object_name(const std::string &name) {
    assert(is_code_object_line(name));
  
    std::string definition = split(split(name, std::regex{ "<" })[1], std::regex{ ">" })[0];
    std::vector<std::string> tokens = split(definition);

    assert(
        tokens[0] == "code" && 
        tokens[1] == "object" && 
        tokens[3] == "at" && 
        tokens[5] == "file" && 
        tokens[7] == "line"
    );

    return tokens[2];
}


Module yapvm::parse(const std::string &module_name, const std::string &program_text) {
    std::vector<std::string> lines = not_matched(split(program_text, std::regex{ "\n" }), std::regex{ "\\s*" });

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
    assert(function_names.size() == function_def_lines.size() + 1);
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