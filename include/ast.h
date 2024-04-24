#pragma once


#include <string>
#include <vector>
#include "utils.h"
#include "y_objects.h"

namespace yapvm::yobjects {
class YObject;
}

namespace yapvm::ast {

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
class USub : public UnaryOpKind {};


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
    WithItem(scoped_ptr<Expr> &&context_expr);
    WithItem(scoped_ptr<Expr> &&context_expr, scoped_ptr<Expr> &&optional_vars);

    const scoped_ptr<Expr> &context_expr() const;
    const scoped_ptr<Expr> &optional_vars() const;

    bool is_optional_var() const;
};


class BoolOp : public Expr {
    scoped_ptr<BoolOpKind> op_;
    std::vector<scoped_ptr<Expr>> values_;

public:
    BoolOp(scoped_ptr<BoolOpKind> &&op, std::vector<scoped_ptr<Expr>> &&values);

    const scoped_ptr<BoolOpKind> &op() const;
    const std::vector<scoped_ptr<Expr>> &values() const;
};


class BinOp : public Expr {
    scoped_ptr<Expr> left_;
    scoped_ptr<BinOpKind> op_;
    scoped_ptr<Expr> right_;

public:
    BinOp(scoped_ptr<Expr> &&left, scoped_ptr<BinOpKind> &&op, scoped_ptr<Expr> &&right);

    const scoped_ptr<Expr> &left() const;
    const scoped_ptr<Expr> &right() const;
    const scoped_ptr<BinOpKind> &op() const;
};

class UnaryOp : public Expr {
    scoped_ptr<UnaryOpKind> op_;
    scoped_ptr<Expr> operand_;

public:
    UnaryOp(scoped_ptr<UnaryOpKind> &&op, scoped_ptr<Expr> &&operand);

    const scoped_ptr<UnaryOpKind> &op() const;
    const scoped_ptr<Expr> &operand() const;
};

class Compare : public Expr {
    scoped_ptr<Expr> left_;
    std::vector<scoped_ptr<CmpOpKind>> ops_;
    std::vector<scoped_ptr<Expr>> comparators_;

public:
    Compare(scoped_ptr<Expr> &&left, std::vector<scoped_ptr<CmpOpKind>> &&ops,
            std::vector<scoped_ptr<Expr>> &&comparators);

    const scoped_ptr<Expr> &left() const;
    const std::vector<scoped_ptr<CmpOpKind>> &ops() const;
    const std::vector<scoped_ptr<Expr>> &comparators() const;
};


class Call : public Expr {
    scoped_ptr<Expr> func_;
    std::vector<scoped_ptr<Expr>> args_;

public:
    Call(scoped_ptr<Expr> &&func, std::vector<scoped_ptr<Expr>> &&args);

    const scoped_ptr<Expr> &func() const;
    const std::vector<scoped_ptr<Expr>> &args() const;
};


class Constant : public Expr {
    scoped_ptr<yobjects::YObject> value_;

public:
    Constant(scoped_ptr<yobjects::YObject> &&value);

    const scoped_ptr<yobjects::YObject> &value() const;
};


class Attribute : public Expr {
    scoped_ptr<Expr> value_;
    std::string attr_;
    scoped_ptr<ExprContext> ctx_;

public:
    Attribute(scoped_ptr<Expr> &&value, std::string &&attr, scoped_ptr<ExprContext> &&ctx);

    const scoped_ptr<ExprContext> &ctx() const;
    const scoped_ptr<Expr> &value() const;
    const std::string &attr() const;
};


class Subscript : public Expr {
    scoped_ptr<Expr> value_;
    scoped_ptr<Expr> key_;
    scoped_ptr<ExprContext> ctx_;

public:
    Subscript(scoped_ptr<Expr> &&value, scoped_ptr<Expr> &&key, scoped_ptr<ExprContext> &&ctx);

    const scoped_ptr<ExprContext> &ctx() const;
    const scoped_ptr<Expr> &key() const;
    const scoped_ptr<Expr> &value() const;
};


class Name : public Expr {
    std::string id_;
    scoped_ptr<ExprContext> ctx_;

public:
    Name(std::string &&id, scoped_ptr<ExprContext> &&ctx);

    const scoped_ptr<ExprContext> &ctx() const;
    const std::string &id() const;
};

class List : public Expr {};
class Dict : public Expr {};


class Stmt : public Node {};


class Module : public Node {
    std::vector<scoped_ptr<Stmt>> body_;

public:
    Module(std::vector<scoped_ptr<Stmt>> &&body);

    const std::vector<scoped_ptr<Stmt>> &body() const;
};


class Import : public Stmt {
    std::string name_;

public:
    Import(std::string &&name);

    const std::string &name() const;
};


class FunctionDef : public Stmt {
    std::string name_;
    std::vector<std::string> args_;
    std::vector<scoped_ptr<Stmt>> body_;
    scoped_ptr<Expr> returns_; // just nullptr if nothing

public:
    FunctionDef(std::string &&name, std::vector<std::string> &&args, std::vector<scoped_ptr<Stmt>> &&body);
    FunctionDef(std::string &&name, std::vector<std::string> &&args, std::vector<scoped_ptr<Stmt>> &&body,
                scoped_ptr<Expr> &&returns);

    const std::string &name() const;
    const std::vector<std::string> &args() const;
    const std::vector<scoped_ptr<Stmt>> &body() const;
    const scoped_ptr<Expr> &returns() const;
    bool returns_anything() const;
};

class ClassDef : public Stmt {
    std::string name_;
    std::vector<scoped_ptr<Stmt>> body_;

public:
    ClassDef(std::string &&name, std::vector<scoped_ptr<Stmt>> &&body);

    const std::string &name() const;
    const std::vector<scoped_ptr<Stmt>> &body() const;
};

class Return : public Stmt {
    scoped_ptr<Expr> value_; // nullptr if returns nothing

public:
    Return(scoped_ptr<Expr> &&value);
    Return();

    bool returns_anything() const;
    const scoped_ptr<Expr> &value() const;
};

class Assign : public Stmt {
    std::vector<scoped_ptr<Expr>> target_;
    scoped_ptr<Expr> value_;

public:
    Assign(std::vector<scoped_ptr<Expr>> &&target, scoped_ptr<Expr> &&value);

    const std::vector<scoped_ptr<Expr>> &target() const;
    const scoped_ptr<Expr> &value() const;
};

class AugAssign : public Stmt {
    scoped_ptr<Expr> target_;
    scoped_ptr<BinOpKind> op_;
    scoped_ptr<Expr> value_;

public:
    AugAssign(scoped_ptr<Expr> &&target, scoped_ptr<BinOpKind> &&op, scoped_ptr<Expr> &&value);

    const scoped_ptr<Expr> &target() const;
    const scoped_ptr<BinOpKind> &op() const;
    const scoped_ptr<Expr> &value() const;
};

class While : public Stmt {
    scoped_ptr<Expr> test_;
    std::vector<scoped_ptr<Stmt>> body_;

public:
    While(scoped_ptr<Expr> &&test, std::vector<scoped_ptr<Stmt>> &&body);

    const scoped_ptr<Expr> &test() const;
    const std::vector<scoped_ptr<Stmt>> &body() const;
};


class For : public Stmt {
    scoped_ptr<Expr> target_;
    scoped_ptr<Expr> iter_;
    std::vector<scoped_ptr<Stmt>> body_;

public:
    For(scoped_ptr<Expr> &&target, scoped_ptr<Expr> &&iter, std::vector<scoped_ptr<Stmt>> &&body);

    const scoped_ptr<Expr> &target() const;
    const scoped_ptr<Expr> &iter() const;
    const std::vector<scoped_ptr<Stmt>> &body() const;
};


class With : public Stmt {
    std::vector<scoped_ptr<WithItem>> items_;
    std::vector<scoped_ptr<Stmt>> body_;

public:
    With(std::vector<scoped_ptr<WithItem>> &&items, std::vector<scoped_ptr<Stmt>> &&body);

    const std::vector<scoped_ptr<WithItem>> &items() const;
    const std::vector<scoped_ptr<Stmt>> &body() const;
};


class If : public Stmt {
    scoped_ptr<Expr> test_;
    std::vector<scoped_ptr<Stmt>> body_;
    std::vector<scoped_ptr<Stmt>> orelse_;

public:
    If(scoped_ptr<Expr> &&test, std::vector<scoped_ptr<Stmt>> &&body, std::vector<scoped_ptr<Stmt>> &&orelse);

    const scoped_ptr<Expr> &test() const;
    const std::vector<scoped_ptr<Stmt>> &body() const;
    const std::vector<scoped_ptr<Stmt>> &orelse() const;
};

class ExprStmt : public Stmt {
    scoped_ptr<Expr> value_;

public:
    ExprStmt(scoped_ptr<Expr> &&value);

    const scoped_ptr<Expr> &value() const;
};

class Pass : public Stmt {};
class Break : public Stmt {};
class Continue : public Stmt {};


} // namespace yapvm::ast
