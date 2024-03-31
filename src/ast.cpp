#include "ast.h"
#include "y_objects.h"


using namespace yapvm::ast;
using namespace yapvm;

yapvm::ast::BoolOp::BoolOp(BoolOpKind op, const array<scoped_ptr<Expr>> &values) 
    : op_{ op }, values_{ values } {}


BoolOpKind yapvm::ast::BoolOp::op() const {
    return op_;
}


const array<scoped_ptr<Expr>> &yapvm::ast::BoolOp::values() const {
    return values_;
}


yapvm::ast::BinOp::BinOp(const scoped_ptr<Expr> &left, OperatorKind op, const scoped_ptr<Expr> &right)
    : left_{ left }, right_{ right }, op_{ op } {
}


OperatorKind yapvm::ast::BinOp::op() const {
    return op_;
}


const scoped_ptr<Expr> &yapvm::ast::BinOp::left() const {
    return left_;
}


const scoped_ptr<Expr> &yapvm::ast::BinOp::right() const {
    return right_;
}


yapvm::ast::UnaryOp::UnaryOp(UnaryOpKind op, const scoped_ptr<Expr> &operand) 
    : op_{ op }, operand_{ operand } {
}


UnaryOpKind yapvm::ast::UnaryOp::op() const {
    return op_;
}


const scoped_ptr<Expr> &yapvm::ast::UnaryOp::operand() const {
    return operand_;
}


yapvm::ast::Compare::Compare(const scoped_ptr<Expr> &left, const array<CmpOpKind> &ops, const array<scoped_ptr<Expr>> &comparators)
    : left_{ left }, ops_{ ops }, comparators_{ comparators } {
}


const scoped_ptr<Expr> &yapvm::ast::Compare::left() const {
    return left_;
}


const array<CmpOpKind> &yapvm::ast::Compare::ops() const {
    return ops_;
}


const array<scoped_ptr<Expr>> &yapvm::ast::Compare::comparators() const {
    return comparators_;
}


yapvm::ast::Call::Call(const scoped_ptr<Expr> &func, const array<scoped_ptr<Expr>> &args)
    : func_{ func }, args_{ args } {
}


const scoped_ptr<Expr> &yapvm::ast::Call::func() const {
    return func_;
}


const array<scoped_ptr<Expr>> &yapvm::ast::Call::args() const {
    return args_;
}


yapvm::ast::Constant::Constant(const scoped_ptr<YPrimitiveObject> &value)
    :value_{ value } {}


const scoped_ptr<YPrimitiveObject> &yapvm::ast::Constant::value() const {
    return value_;
}

yapvm::ast::Attribute::Attribute(const scoped_ptr<Expr> &value, const std::string &attr)
    : value_{ value }, attr_{ attr } {
}


const scoped_ptr<Expr> &yapvm::ast::Attribute::value() const {
    return value_;
}


const std::string &yapvm::ast::Attribute::attr() const {
    return attr_;
}


yapvm::ast::Subscript::Subscript(const scoped_ptr<Expr> &value, const scoped_ptr<Expr> &key)
    : key_{ key }, value_{ value } {
}


const scoped_ptr<Expr> &yapvm::ast::Subscript::key() const {
    return key_;
}

const 
scoped_ptr<Expr> &yapvm::ast::Subscript::value() const {
    return value_;
}


yapvm::ast::Name::Name(const std::string &id)
    : id_{ id } {
}


const std::string &yapvm::ast::Name::id() const {
    return id_;
}

yapvm::ast::FunctionDef::FunctionDef(const std::string &name, const array<std::string> &args, const array<scoped_ptr<Stmt>> &body) 
    : name_{ name }, args_{ args }, body_{ body }, returns_{ nullptr } {
}


yapvm::ast::FunctionDef::FunctionDef(const std::string &name, const array<std::string> &args, const array<scoped_ptr<Stmt>> &body, const scoped_ptr<Expr> &returns)
    : name_{ name }, args_{ args }, body_{ body }, returns_{ returns } {}


const std::string &yapvm::ast::FunctionDef::name() const {
    return name_;
}


const array<std::string> &yapvm::ast::FunctionDef::args() const {
    return args_;
}


const array<scoped_ptr<Stmt>> &yapvm::ast::FunctionDef::body() const {
    return body_;
}


bool yapvm::ast::FunctionDef::returns_anything() const {
    return returns_ != nullptr;
}


const scoped_ptr<Expr> &yapvm::ast::FunctionDef::returns() const {
    return returns_;
}


yapvm::ast::ClassDef::ClassDef(const std::string &name, const array<scoped_ptr<Stmt>> &body)
    : name_{ name }, body_{ body } {}


const std::string &yapvm::ast::ClassDef::name() const {
    return name_;
}


const array<scoped_ptr<Stmt>> &yapvm::ast::ClassDef::body() const {
    return body_;
}

yapvm::ast::Return::Return(const scoped_ptr<Expr> &value)
    : value_{ value } {}


bool yapvm::ast::Return::returns_anything() const {
    return value_ != nullptr;
}


const scoped_ptr<Expr> &yapvm::ast::Return::value() const {
    return value_;
}


yapvm::ast::Assign::Assign(const scoped_ptr<Expr> &target, const scoped_ptr<Expr> &value)
    : target_{ target }, value_{ value } {
}


const scoped_ptr<Expr> &yapvm::ast::Assign::target() const {
    return target_;
}


const scoped_ptr<Expr> &yapvm::ast::Assign::value() const {
    return value_;
}


yapvm::ast::AugAssign::AugAssign(const scoped_ptr<Expr> &target, OperatorKind op, const scoped_ptr<Expr> &value)
    : target_{ target }, op_{ op }, value_{ value } {
}


const scoped_ptr<Expr> &yapvm::ast::AugAssign::target() const {
    return target_;
}


OperatorKind yapvm::ast::AugAssign::op() const {
    return op_;
}


const scoped_ptr<Expr> &yapvm::ast::AugAssign::value() const {
    return value_;
}


yapvm::ast::While::While(const scoped_ptr<Expr> &test, const array<scoped_ptr<Stmt>> &body)
    : test_{ test }, body_{ body } {
}


const scoped_ptr<Expr> &yapvm::ast::While::test() const {
    return test_;
}


const array<scoped_ptr<Stmt>> &yapvm::ast::While::body() const {
    return body_;
}


yapvm::ast::If::If(const scoped_ptr<Expr> &test, const array<scoped_ptr<Stmt>> &body, const array<scoped_ptr<Stmt>> &orelse)
    : test_{ test }, body_{ body }, orelse_{ orelse } {
}


const scoped_ptr<Expr> &yapvm::ast::If::test() const {
    return test_;
}


const array<scoped_ptr<Stmt>> &yapvm::ast::If::body() const {
    return body_;
}


const array<scoped_ptr<Stmt>> &yapvm::ast::If::orelse() const {
    return orelse_;
}


yapvm::ast::ExprStmt::ExprStmt(const scoped_ptr<Expr> &value)
    : value_{ value } {
}


const scoped_ptr<Expr> &yapvm::ast::ExprStmt::value() const {
    return value_;
}


yapvm::ast::Module::Module(const array<scoped_ptr<Stmt>> &body) 
    : body_{ body } {}



const array<scoped_ptr<Stmt>> &yapvm::ast::Module::body() const {
    return body_;
}


yapvm::ast::Import::Import(const std::string &name) : name_{ name }{}


const std::string &yapvm::ast::Import::name() const {
    return name_;
}