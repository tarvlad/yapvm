#include "ast.h"
#include "y_objects.h"


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


yapvm::ast::BinOp::BinOp(Expr *left, OperatorKind op, Expr *right) 
    : left_{ left }, right_{ right }, op_{ op } {
}


yapvm::ast::BinOp::~BinOp() {
    delete left_;
    delete right_;
}


OperatorKind yapvm::ast::BinOp::op() const {
    return op_;
}


Expr *yapvm::ast::BinOp::left() const {
    return left_;
}


Expr *yapvm::ast::BinOp::right() const {
    return left_;
}


yapvm::ast::UnaryOp::UnaryOp(UnaryOpKind op, Expr *operand) 
    : op_{ op }, operand_{ operand } {
}


yapvm::ast::UnaryOp::~UnaryOp() {
    delete operand_;
}


UnaryOpKind yapvm::ast::UnaryOp::op() const {
    return op_;
}


Expr *yapvm::ast::UnaryOp::operand() const {
    return operand_;
}


yapvm::ast::Compare::Compare(Expr *left, std::span<CmpOpKind> ops, std::span<Expr *> comparators) 
    : left_{ left }, ops_{ ops }, comparators_{ comparators } {
}


yapvm::ast::Compare::~Compare() {
    delete left_;
    delete[] ops_.data();
    for (Expr *e : comparators_) {
        delete e;
    }
    delete[] comparators_.data();
}


const std::span<CmpOpKind> &yapvm::ast::Compare::ops() const {
    return ops_;
}


const std::span<Expr *> &yapvm::ast::Compare::comparators() const {
    return comparators_;
}


yapvm::ast::Call::Call(Expr *func, std::span<Expr *> args) 
    : func_{ func }, args_{ args } {
}


yapvm::ast::Call::~Call() {
    delete func_;
    for (Expr *e : args_) {
        delete e;
    }
    delete[] args_.data();
}


Expr *yapvm::ast::Call::func() const {
    return func_;
}


const std::span<Expr *> &yapvm::ast::Call::args() const {
    return args_;
}


yapvm::ast::Constant::Constant(YPrimitiveObject *value) 
    : value_{ value } {
}


yapvm::ast::Constant::~Constant() {
    delete value_;
}


YPrimitiveObject *yapvm::ast::Constant::value() const {
    return value_;
}


yapvm::ast::Attribute::Attribute(Expr *value, const std::string &attr)
    : value_{ value }, attr_{ attr } {
}


yapvm::ast::Attribute::~Attribute() {
    delete value_;
}


Expr *yapvm::ast::Attribute::value() const {
    return value_;
}


const std::string &yapvm::ast::Attribute::attr() const {
    return attr_;
}


yapvm::ast::Subscript::Subscript(Expr *value, Expr *key)
    : key_{ key }, value_{ value } {
}


yapvm::ast::Subscript::~Subscript() {
    delete key_;
    delete value_;
}


Expr *yapvm::ast::Subscript::key() const {
    return key_;
}


Expr *yapvm::ast::Subscript::value() const {
    return value_;
}


yapvm::ast::Name::Name(const std::string &id)
    : id_{ id } {
}


const std::string &yapvm::ast::Name::id() const {
    return id_;
}


yapvm::ast::FunctionDef::FunctionDef(const std::string &name, std::span<std::string> args, std::span<Stmt *> body) 
    : name_{ name }, args_{ args }, body_{ body }, returns_{ nullptr } {
}

yapvm::ast::FunctionDef::FunctionDef(const std::string &name, std::span<std::string> args, std::span<Stmt *> body, Expr *returns)
    : name_{ name }, args_{ args }, body_{ body }, returns_{ returns } {
}

yapvm::ast::FunctionDef::~FunctionDef() {
    delete[] args_.data();
    for (Stmt *s : body_) {
        delete s;
    }
    delete[] body_.data();
    delete returns_;
}


const std::string &yapvm::ast::FunctionDef::name() const {
    return name_;
}


const std::span<std::string> &yapvm::ast::FunctionDef::args() const {
    return args_;
}


const std::span<Stmt *> &yapvm::ast::FunctionDef::body() const {
    return body_;
}


bool yapvm::ast::FunctionDef::returns_anything() const {
    return returns_ != nullptr;
}


Expr *yapvm::ast::FunctionDef::returns() const {
    return returns_;
}


yapvm::ast::ClassDef::ClassDef(const std::string &name, std::span<Stmt *> body) 
    : name_{ name }, body_{ body } {
}


yapvm::ast::ClassDef::~ClassDef() {
    for (Stmt *s : body_) {
        delete s;
    }
    delete[] body_.data();
}


const std::string &yapvm::ast::ClassDef::name() const {
    return name_;
}


const std::span<Stmt *> yapvm::ast::ClassDef::body() const {
    return body_;
}


yapvm::ast::Return::Return(Expr *value) 
    : value_{ value } {
}


yapvm::ast::Return::~Return() {
    delete value_;
}


bool yapvm::ast::Return::returns_anything() const {
    return value_ != nullptr;
}


Expr *yapvm::ast::Return::value() const {
    return value_;
}


yapvm::ast::Assign::Assign(Expr *target, Expr *value)
    : target_{ target }, value_{ value } {
}


yapvm::ast::Assign::~Assign() {
    delete target_;
    delete value_;
}


Expr *yapvm::ast::Assign::target() const {
    return target_;
}


Expr *yapvm::ast::Assign::value() const {
    return value_;
}


yapvm::ast::AugAssign::AugAssign(Expr *target, OperatorKind op, Expr *value) 
    : target_{ target }, op_{ op }, value_{ value } {
}


yapvm::ast::AugAssign::~AugAssign() {
    delete target_;
    delete value_;
}


Expr *yapvm::ast::AugAssign::target() const {
    return target_;
}


OperatorKind yapvm::ast::AugAssign::op() const {
    return op_;
}


Expr *yapvm::ast::AugAssign::value() const {
    return value_;
}


yapvm::ast::While::While(Expr *test, std::span<Stmt *> body) 
    : test_{ test }, body_{ body } {
}


yapvm::ast::While::~While() {
    delete test_;
    for (Stmt *s : body_) {
        delete s;
    }
    delete[] body_.data();
}


Expr *yapvm::ast::While::test() const {
    return test_;
}


const std::span<Stmt *> &yapvm::ast::While::body() const {
    return body_;
}


yapvm::ast::If::If(Expr *test, std::span<Stmt *> body, std::span<Stmt *> orelse) 
    : test_{ test }, body_{ body }, orelse_{ orelse } {
}


yapvm::ast::If::~If() {
    delete test_;
    for (Stmt *s : body_) {
        delete s;
    }
    delete[] body_.data();
    for (Stmt *s : orelse_) {
        delete s;
    }
    delete[] orelse_.data();
}


Expr *yapvm::ast::If::test() const {
    return test_;
}


const std::span<Stmt *> &yapvm::ast::If::body() const {
    return body_;
}


const std::span<Stmt *> &yapvm::ast::If::orelse() const {
    return orelse_;
}


yapvm::ast::ExprStmt::ExprStmt(Expr *value)
    : value_{ value } {
}


yapvm::ast::ExprStmt::~ExprStmt() {
    delete value_;
}


Expr *yapvm::ast::ExprStmt::value() const {
    return value_;
}
