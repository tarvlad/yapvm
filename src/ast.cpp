#include "ast.h"
#include "y_objects.h"


using namespace yapvm::ast;
using namespace yapvm;


yapvm::ast::BoolOp::BoolOp(scoped_ptr<BoolOpKind> &&op, std::vector<scoped_ptr<Expr>> &&values) 
    : op_{ std::move(op) }, values_{ std::move(values) } {}


const scoped_ptr<BoolOpKind> &yapvm::ast::BoolOp::op() const {
    return op_;
}


const std::vector<scoped_ptr<Expr>> &yapvm::ast::BoolOp::values() const {
    return values_;
}


yapvm::ast::BinOp::BinOp(scoped_ptr<Expr> &&left, scoped_ptr<BinOpKind> &&op, scoped_ptr<Expr> &&right)
    : left_{ std::move(left) }, right_{ std::move(right) }, op_{ std::move(op) } {
}


const scoped_ptr<BinOpKind> &yapvm::ast::BinOp::op() const {
    return op_;
}


const scoped_ptr<Expr> &yapvm::ast::BinOp::left() const {
    return left_;
}


const scoped_ptr<Expr> &yapvm::ast::BinOp::right() const {
    return right_;
}


yapvm::ast::UnaryOp::UnaryOp(scoped_ptr<UnaryOpKind> &&op, scoped_ptr<Expr> &&operand)
    : op_{ std::move(op) }, operand_{ std::move(operand) } {
}


const scoped_ptr<UnaryOpKind> &yapvm::ast::UnaryOp::op() const {
    return op_;
}


const scoped_ptr<Expr> &yapvm::ast::UnaryOp::operand() const {
    return operand_;
}


yapvm::ast::Compare::Compare(scoped_ptr<Expr> &&left, std::vector<scoped_ptr<CmpOpKind>> &&ops, std::vector<scoped_ptr<Expr>> &&comparators)
    : left_{ std::move(left) }, ops_{ std::move(ops) }, comparators_{ std::move(comparators) } {
}


const scoped_ptr<Expr> &yapvm::ast::Compare::left() const {
    return left_;
}


const std::vector<scoped_ptr<CmpOpKind>> &yapvm::ast::Compare::ops() const {
    return ops_;
}


const std::vector<scoped_ptr<Expr>> &yapvm::ast::Compare::comparators() const {
    return comparators_;
}


yapvm::ast::Call::Call(scoped_ptr<Expr> &&func, std::vector<scoped_ptr<Expr>> &&args)
    : func_{ std::move(func) }, args_{ std::move(args) } {
}


const scoped_ptr<Expr> &yapvm::ast::Call::func() const {
    return func_;
}


const std::vector<scoped_ptr<Expr>> &yapvm::ast::Call::args() const {
    return args_;
}


yapvm::ast::Constant::Constant(scoped_ptr<YPrimitiveObject> &&value)
    :value_{ std::move(value) } {}


const scoped_ptr<YPrimitiveObject> &yapvm::ast::Constant::value() const {
    return value_;
}

yapvm::ast::Attribute::Attribute(scoped_ptr<Expr> &&value, std::string &&attr, scoped_ptr<ExprContext> &&ctx)
    : value_{ std::move(value) }, attr_{ std::move(attr) }, ctx_{ std::move(ctx) } {
}


const scoped_ptr<ExprContext> &yapvm::ast::Attribute::ctx() const {
    return ctx_;
}


const scoped_ptr<Expr> &yapvm::ast::Attribute::value() const {
    return value_;
}


const std::string &yapvm::ast::Attribute::attr() const {
    return attr_;
}


yapvm::ast::Subscript::Subscript(scoped_ptr<Expr> &&value, scoped_ptr<Expr> &&key, scoped_ptr<ExprContext> &&ctx)
    : key_{ std::move(key) }, value_{ std::move(value) }, ctx_{ std::move(ctx) } {
}


const scoped_ptr<ExprContext> &yapvm::ast::Subscript::ctx() const {
    return ctx_;
}


const scoped_ptr<Expr> &yapvm::ast::Subscript::key() const {
    return key_;
}

const 
scoped_ptr<Expr> &yapvm::ast::Subscript::value() const {
    return value_;
}


yapvm::ast::Name::Name(std::string &&id, scoped_ptr<ExprContext> &&ctx)
    : id_{ std::move(id) }, ctx_{ std::move(ctx) } {
}


const scoped_ptr<ExprContext> &yapvm::ast::Name::ctx() const {
    return ctx_;
}


const std::string &yapvm::ast::Name::id() const {
    return id_;
}


yapvm::ast::FunctionDef::FunctionDef(std::string &&name, std::vector<std::string> &&args, std::vector<scoped_ptr<Stmt>> &&body) 
    : name_{ std::move(name) }, args_{ std::move(args) }, body_{ std::move(body) }, returns_{ nullptr } {
}


yapvm::ast::FunctionDef::FunctionDef(std::string &&name, std::vector<std::string> &&args, std::vector<scoped_ptr<Stmt>> &&body, scoped_ptr<Expr> &&returns)
    : name_{ std::move(name) }, args_{ std::move(args) }, body_{ std::move(body) }, returns_{ std::move(returns) } {}


const std::string &yapvm::ast::FunctionDef::name() const {
    return name_;
}


const std::vector<std::string> &yapvm::ast::FunctionDef::args() const {
    return args_;
}


const std::vector<scoped_ptr<Stmt>> &yapvm::ast::FunctionDef::body() const {
    return body_;
}


bool yapvm::ast::FunctionDef::returns_anything() const {
    return returns_ != nullptr;
}


const scoped_ptr<Expr> &yapvm::ast::FunctionDef::returns() const {
    return returns_;
}


yapvm::ast::ClassDef::ClassDef(std::string &&name, std::vector<scoped_ptr<Stmt>> &&body)
    : name_{ std::move(name) }, body_{ std::move(body) } {}


const std::string &yapvm::ast::ClassDef::name() const {
    return name_;
}


const std::vector<scoped_ptr<Stmt>> &yapvm::ast::ClassDef::body() const {
    return body_;
}

yapvm::ast::Return::Return(scoped_ptr<Expr> &&value)
    : value_{ std::move(value) } {}


yapvm::ast::Return::Return() : value_{ nullptr } {}


bool yapvm::ast::Return::returns_anything() const {
    return value_ != nullptr;
}


const scoped_ptr<Expr> &yapvm::ast::Return::value() const {
    return value_;
}


yapvm::ast::Assign::Assign(std::vector<scoped_ptr<Expr>> &&target, scoped_ptr<Expr> &&value)
    : target_{ std::move(target) }, value_{ std::move(value) } {
}


const std::vector<scoped_ptr<Expr>> &yapvm::ast::Assign::target() const {
    return target_;
}


const scoped_ptr<Expr> &yapvm::ast::Assign::value() const {
    return value_;
}


yapvm::ast::AugAssign::AugAssign(scoped_ptr<Expr> &&target, scoped_ptr<BinOpKind> &&op, scoped_ptr<Expr> &&value)
    : target_{ std::move(target) }, op_{ std::move(op) }, value_{ std::move(value) } {
}


const scoped_ptr<Expr> &yapvm::ast::AugAssign::target() const {
    return target_;
}


const scoped_ptr<BinOpKind> &yapvm::ast::AugAssign::op() const {
    return op_;
}


const scoped_ptr<Expr> &yapvm::ast::AugAssign::value() const {
    return value_;
}


yapvm::ast::While::While(scoped_ptr<Expr> &&test, std::vector<scoped_ptr<Stmt>> &&body)
    : test_{ std::move(test) }, body_{ std::move(body) } {
}


const scoped_ptr<Expr> &yapvm::ast::While::test() const {
    return test_;
}


const std::vector<scoped_ptr<Stmt>> &yapvm::ast::While::body() const {
    return body_;
}


yapvm::ast::If::If(scoped_ptr<Expr> &&test, std::vector<scoped_ptr<Stmt>> &&body, std::vector<scoped_ptr<Stmt>> &&orelse)
    : test_{ std::move(test) }, body_{ std::move(body) }, orelse_{ std::move(orelse) } {
}


const scoped_ptr<Expr> &yapvm::ast::If::test() const {
    return test_;
}


const std::vector<scoped_ptr<Stmt>> &yapvm::ast::If::body() const {
    return body_;
}


const std::vector<scoped_ptr<Stmt>> &yapvm::ast::If::orelse() const {
    return orelse_;
}


yapvm::ast::ExprStmt::ExprStmt(scoped_ptr<Expr> &&value)
    : value_{ std::move(value) } {
}


const scoped_ptr<Expr> &yapvm::ast::ExprStmt::value() const {
    return value_;
}


yapvm::ast::Module::Module(std::vector<scoped_ptr<Stmt>> &&body) 
    : body_{ std::move(body) } {}



const std::vector<scoped_ptr<Stmt>> &yapvm::ast::Module::body() const {
    return body_;
}


yapvm::ast::Import::Import(std::string &&name) : name_{ std::move(name) }{}


const std::string &yapvm::ast::Import::name() const {
    return name_;
}


yapvm::ast::For::For(scoped_ptr<Expr> &&target, scoped_ptr<Expr> &&iter, std::vector<scoped_ptr<Stmt>> &&body) 
    : target_{ std::move(target) }, iter_{ std::move(iter) }, body_{ std::move(body) } {
}


const scoped_ptr<Expr> &yapvm::ast::For::target() const {
    return target_;
}


const scoped_ptr<Expr> &yapvm::ast::For::iter() const {
    return iter_;;
}


const std::vector<scoped_ptr<Stmt>> &yapvm::ast::For::body() const {
    return body_;
}


yapvm::ast::WithItem::WithItem(scoped_ptr<Expr> &&context_expr)
    : context_expr_{ std::move(context_expr) }, optional_vars_{ nullptr } {
}


yapvm::ast::WithItem::WithItem(scoped_ptr<Expr> &&context_expr, scoped_ptr<Expr> &&optional_vars)
    : context_expr_{ std::move(context_expr) }, optional_vars_{ std::move(optional_vars) } {
}


const scoped_ptr<Expr> &yapvm::ast::WithItem::context_expr() const {
    return context_expr_;
}


const scoped_ptr<Expr> &yapvm::ast::WithItem::optional_vars() const {
    return optional_vars_;
}


bool yapvm::ast::WithItem::is_optional_var() const {
    return optional_vars_ != nullptr;
}


yapvm::ast::With::With(std::vector<scoped_ptr<WithItem>> &&items, std::vector<scoped_ptr<Stmt>> &&body)
    : items_{ std::move(items) }, body_{ std::move(body) } {}


const std::vector<scoped_ptr<WithItem>> &yapvm::ast::With::items() const {
    return items_;
}


const std::vector<scoped_ptr<Stmt>> &yapvm::ast::With::body() const {
    return body_;
}