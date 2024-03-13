#include "ast.h"


using namespace yapvm::ast;


yapvm::ast::BoolOp::BoolOp(BoolOpKind op, std::span<Expr *> values)
    : op_{ op }, values_{ values } {
}


yapvm::ast::BoolOp::~BoolOp() {
    for (Expr *e : values_) {
        if (e != nullptr) {
            delete e;
        }
    }

    delete[] values_.data();
}


BoolOpKind yapvm::ast::BoolOp::op() const {
    return op_;
}


const std::span<Expr *> &yapvm::ast::BoolOp::values() const {
    return values_;
}