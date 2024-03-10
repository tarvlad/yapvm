#include "ast.h"

using namespace yapvm::ast;


CmpOpKind yapvm::ast::Eq::kind() const {
    return EQ;
}


CmpOpKind yapvm::ast::NotEq::kind() const {
    return NOT_EQ;
}


CmpOpKind yapvm::ast::Lt::kind() const {
    return LT;
}


CmpOpKind yapvm::ast::LtE::kind() const {
    return LTE;
}


CmpOpKind yapvm::ast::Gt::kind() const {
    return GT;
}


CmpOpKind yapvm::ast::GtE::kind() const {
    return GTE;
}
