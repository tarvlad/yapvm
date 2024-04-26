#pragma once

#include "ast.h"


namespace yapvm::interpreter {
using namespace yapvm::ast;


class Interpreter {


public:
    Interpreter(Module *code);

    void park();
    bool is_parked();
    void run();
};


}
