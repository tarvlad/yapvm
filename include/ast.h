#pragma once


#include <span>
#include "y_objects.h"
#include <string>
#include <vector>
#include "utils.h"


namespace yapvm {
namespace ast {

using namespace yapvm::yobjects;

class Node {
public:
    virtual ~Node() = default;
};


class OperatorKind : public Node {};


class CmpOpKind : public OperatorKind {};

class Eq : public CmpOpKind {};
class NotEq : public CmpOpKind {};
class Lt : public CmpOpKind {};
class LtE : public CmpOpKind {};
class Gt : public CmpOpKind {};
class GtE : public CmpOpKind {};
class Is : public CmpOpKind {};
class IsNot : public CmpOpKind {};
class In : public CmpOpKind {};
class NotIn : public CmpOpKind {};


class UnaryOpKind : public OperatorKind {};

class Invert : public UnaryOpKind {};
class Not : public UnaryOpKind {};


class BinOpKind : public OperatorKind {};

class Add : public BinOpKind {};
class Sub : public BinOpKind {};
class Mult : public BinOpKind {};
class Div : public BinOpKind {};
class Mod : public BinOpKind {};
class Pow : public BinOpKind {};
class LShift : public BinOpKind {};
class RShift : public BinOpKind {};
class BitOr : public BinOpKind {};
class BitXor : public BinOpKind {};
class BitAnd : public BinOpKind {};
class FloorDiv : public BinOpKind {};


class BoolOpKind : public OperatorKind {};

class And : public BoolOpKind {};
class Or : public BoolOpKind {};


class ExprContext : public Node {};

class Load : public ExprContext {};
class Store : public ExprContext {};
class Del : public ExprContext {};


class Expr : public Node {};


class WithItem : public Node {
    scoped_ptr<Expr> context_expr_;
    scoped_ptr<Expr> optional_vars_;

public:
    WithItem(const scoped_ptr<Expr> &context_expr);
    WithItem(const scoped_ptr<Expr> &context_expr, const scoped_ptr<Expr> &optional_vars);

    const scoped_ptr<Expr> &context_expr() const;
    const scoped_ptr<Expr> &optional_vars() const;
    bool is_optional_var() const;
};


class BoolOp : public Expr {
    scoped_ptr<BoolOpKind> op_;
    array<scoped_ptr<Expr>> values_;

public:
    BoolOp(const scoped_ptr<BoolOpKind> &op, const array<scoped_ptr<Expr>> &values);

    const scoped_ptr<BoolOpKind> &op() const;
    const array<scoped_ptr<Expr>> &values() const;
};


class BinOp : public Expr {
    scoped_ptr<Expr> left_;
    scoped_ptr<BinOpKind> op_;
    scoped_ptr<Expr> right_;

public:
    BinOp(const scoped_ptr<Expr> &left, const scoped_ptr<BinOpKind> &op, const scoped_ptr<Expr> &right);

    const scoped_ptr<Expr> &left() const;
    const scoped_ptr<Expr> &right() const;
    const scoped_ptr<BinOpKind> &op() const;
};

class UnaryOp : public Expr {
    scoped_ptr<UnaryOpKind> op_;
    scoped_ptr<Expr> operand_;

public:
    UnaryOp(const scoped_ptr<UnaryOpKind> &op, const scoped_ptr<Expr> &operand);

    const scoped_ptr<UnaryOpKind> &op() const;
    const scoped_ptr<Expr> &operand() const;
};

class Compare : public Expr {
    scoped_ptr<Expr> left_;
    array<scoped_ptr<CmpOpKind>> ops_;
    array<scoped_ptr<Expr>> comparators_;

public:
    Compare(
        const scoped_ptr<Expr> &left, 
        const array<scoped_ptr<CmpOpKind>> &ops, 
        const array<scoped_ptr<Expr>> &comparators
    );

    const scoped_ptr<Expr> &left() const;
    const array<scoped_ptr<CmpOpKind>> &ops() const;
    const array<scoped_ptr<Expr>> &comparators() const;
};


class Call : public Expr {
    scoped_ptr<Expr> func_;
    array<scoped_ptr<Expr>> args_;

public:
    Call(const scoped_ptr<Expr> &func, const array<scoped_ptr<Expr>> &args);

    const scoped_ptr<Expr> &func() const;
    const array<scoped_ptr<Expr>> &args() const;
};


class Constant : public Expr {
    scoped_ptr<YPrimitiveObject> value_;

public:
    Constant(const scoped_ptr<YPrimitiveObject> &value);

    const scoped_ptr<YPrimitiveObject> &value() const;
};


class Attribute : public Expr {
    scoped_ptr<Expr> value_;
    std::string attr_;
    scoped_ptr<ExprContext> ctx_;

public:
    Attribute(const scoped_ptr<Expr> &value, const std::string &attr, const scoped_ptr<ExprContext> &ctx);

    const scoped_ptr<ExprContext> &ctx() const;
    const scoped_ptr<Expr> &value() const;
    const std::string &attr() const;
};


class Subscript : public Expr {
    scoped_ptr<Expr> value_;
    scoped_ptr<Expr> key_;
    scoped_ptr<ExprContext> ctx_;

public:
    Subscript(const scoped_ptr<Expr> &value, const scoped_ptr<Expr> &key, const scoped_ptr<ExprContext> &ctx);

    const scoped_ptr<ExprContext> &ctx() const;
    const scoped_ptr<Expr> &key() const;
    const scoped_ptr<Expr> &value() const;
};


class Name : public Expr {
    std::string id_;
    scoped_ptr<ExprContext> ctx_;

public:
    Name(const std::string &id, const scoped_ptr<ExprContext> &ctx);

    const scoped_ptr<ExprContext> &ctx() const;
    const std::string &id() const;
};

class List : public Expr {};
class Dict : public Expr {};


class Stmt : public Node {};


class Module : public Node {
    array<scoped_ptr<Stmt>> body_;
    
public:
    Module(const array<scoped_ptr<Stmt>> &body);

    const array<scoped_ptr<Stmt>> &body() const;
};


class Import : public Stmt {
    std::string name_;

public:
    Import(const std::string &name);

    const std::string &name() const;
};


class FunctionDef : public Stmt {
    std::string name_;
    array<std::string> args_;
    array<scoped_ptr<Stmt>> body_;
    scoped_ptr<Expr> returns_; // just nullptr if nothing

public:
    FunctionDef(const std::string &name, const array<std::string> &args, const array<scoped_ptr<Stmt>> &body);
    FunctionDef(const std::string &name, const array<std::string> &args, const array<scoped_ptr<Stmt>> &body, const scoped_ptr<Expr> &returns);

    const std::string &name() const;
    const array<std::string> &args() const;
    const array<scoped_ptr<Stmt>> &body() const;
    const scoped_ptr<Expr> &returns() const;
    bool returns_anything() const;
};

class ClassDef : public Stmt {
    std::string name_;
    array<scoped_ptr<Stmt>> body_;

public:
    ClassDef(const std::string &name, const array<scoped_ptr<Stmt>> &body);

    const std::string &name() const;
    const array<scoped_ptr<Stmt>> &body() const;
};

class Return : public Stmt {
    scoped_ptr<Expr> value_; // nullptr if returns nothing

public:
    Return(const scoped_ptr<Expr> &value);
    Return();

    bool returns_anything() const;
    const scoped_ptr<Expr> &value() const;
};

class Assign : public Stmt {
    array<scoped_ptr<Expr>> target_;
    scoped_ptr<Expr> value_;

public:
    Assign(const array<scoped_ptr<Expr>> &target, const scoped_ptr<Expr> &value);

    const array<scoped_ptr<Expr>> &target() const;
    const scoped_ptr<Expr> &value() const;
};

class AugAssign : public Stmt {
    scoped_ptr<Expr> target_;
    scoped_ptr<BinOpKind> op_;
    scoped_ptr<Expr> value_;

public:
    AugAssign(const scoped_ptr<Expr> &target, const scoped_ptr<BinOpKind> &op, const scoped_ptr<Expr> &value);

    const scoped_ptr<Expr> &target() const;
    const scoped_ptr<BinOpKind> &op() const;
    const scoped_ptr<Expr> &value() const;
};

class While : public Stmt {
    scoped_ptr<Expr> test_;
    array<scoped_ptr<Stmt>> body_;

public:
    While(const scoped_ptr<Expr> &test, const array<scoped_ptr<Stmt>> &body);

    const scoped_ptr<Expr> &test() const;
    const array<scoped_ptr<Stmt>> &body() const;
};


class For : public Stmt {
    scoped_ptr<Expr> target_;
    scoped_ptr<Expr> iter_;
    array<scoped_ptr<Stmt>> body_;

public:
    For(const scoped_ptr<Expr> &target, const scoped_ptr<Expr> &iter, const array<scoped_ptr<Stmt>> &body);

    const scoped_ptr<Expr> &target() const;
    const scoped_ptr<Expr> &iter() const;
    const array<scoped_ptr<Stmt>> &body() const;
};


class With : public Stmt {
    array<scoped_ptr<WithItem>> items_;
    array<scoped_ptr<Stmt>> body_;

public:
    With(const array<scoped_ptr<WithItem>> &items, const array<scoped_ptr<Stmt>> &body);

    const array<scoped_ptr<WithItem>> &items() const;
    const array<scoped_ptr<Stmt>> &body() const;
};


class If : public Stmt {
    scoped_ptr<Expr> test_;
    array<scoped_ptr<Stmt>> body_;
    array<scoped_ptr<Stmt>> orelse_;

public:
    If(const scoped_ptr<Expr> &test, const array<scoped_ptr<Stmt>> &body, const array<scoped_ptr<Stmt>> &orelse);

    const scoped_ptr<Expr> &test() const;
    const array<scoped_ptr<Stmt>> &body() const;
    const array<scoped_ptr<Stmt>> &orelse() const;
};

class ExprStmt : public Stmt {
    scoped_ptr<Expr> value_;

public:
    ExprStmt(const scoped_ptr<Expr> &value);

    const scoped_ptr<Expr> &value() const;
};

class Pass : public Stmt {};
class Break : public Stmt {};
class Continue : public Stmt {};


} // namespace ast
} // namespace yapvm