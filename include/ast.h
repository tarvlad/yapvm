#pragma once


#include <span>
#include "y_objects.h"
#include <string>


namespace yapvm {
namespace ast {

using namespace yapvm::yobjects;

class Node {
public:
    virtual ~Node() = default;
};


class CmpOpKind : public Node {};

class Eq : public CmpOpKind {};
class NotEq : public CmpOpKind {};
class Lt : public CmpOpKind {};
class LtE : public CmpOpKind {};
class Gt : public CmpOpKind {};
class GtE : public CmpOpKind {};


class UnaryOpKind : public Node {};

class Invert : public Node {};
class Not : public Node {};


class OperatorKind : public Node {};

class Add : public OperatorKind {};
class Sub : public OperatorKind {};
class Mult : public OperatorKind {};
class Div : public OperatorKind {};
class Mod : public OperatorKind {};
class Pow : public OperatorKind {};
class LShift : public OperatorKind {};
class RShift : public OperatorKind {};
class BitOr : public OperatorKind {};
class BitXor : public OperatorKind {};
class BitAnd : public OperatorKind {};
class FloorDiv : public OperatorKind {};


class BoolOpKind : public Node {};

class And : public BoolOpKind {};
class Or : public BoolOpKind {};


class ExprContext : public Node {};

class Load : public ExprContext {};
class Store : public ExprContext {};


class Expr : public Node {};

class BoolOp : public Expr {
    BoolOpKind op_;
    std::span<Expr *> values_;

public:
    BoolOp(BoolOpKind op, std::span<Expr *> values);
    ~BoolOp();

    BoolOpKind op() const;
    const std::span<Expr *> &values() const;
};

class BinOp : public Expr {
    Expr *left_;
    OperatorKind op_;
    Expr *right_;

public:
    BinOp(Expr *left, OperatorKind op, Expr *right);
    ~BinOp();

    Expr *left() const;
    Expr *right() const;
    OperatorKind op() const;
};

class UnaryOp : public Expr {
    UnaryOpKind op_;
    Expr *operand_;

public:
    UnaryOp(UnaryOpKind op, Expr *operand);
    ~UnaryOp();

    UnaryOpKind op() const;
    Expr *operand() const;
};

class Dict : public Expr {
    std::span<Expr *> keys_;
    std::span<Expr *> values_;

public:
    Dict(std::span<Expr *> keys, std::span<Expr *> values);
    ~Dict();

    const std::span<Expr *> &keys() const;
    const std::span<Expr *> &values() const;
};


class Set : public Expr {
    std::span<Expr *> elts_;

public:
    Set(std::span<Expr *> elts);
    ~Set();

    const std::span<Expr *> &elts();
};


class Compare : public Expr {
    Expr *left_;
    std::span<CmpOpKind> ops_;
    std::span<Expr *> comparators_;

public:
    Compare(Expr *left, std::span<CmpOpKind> ops, std::span<Expr *> comparators);
    ~Compare();

    const std::span<CmpOpKind> &ops() const;
    const std::span<Expr *> &comparators() const;
};


class Call : public Expr {
    Expr *func_;
    std::span<Expr *> args_;

public:
    Call(Expr *func, std::span<Expr *> args);
    ~Call();

    Expr *func() const;
    const std::span<Expr *> &args() const;
};


class Constant : public Expr {
    YObject *value_;

public:
    Constant(YObject *value);
    ~Constant();

    YObject *value() const;
};


class Attribute : public Expr {
    Expr *value_;
    std::string attr_;
    ExprContext ctx_;

public:
    Attribute(Expr *value, const std::string &attr, ExprContext ctx);
    ~Attribute();

    Expr *value() const;
    const std::string &attr() const;
    ExprContext ctx() const;
};


class Subscript : public Expr {
    Expr *value_;
    Expr *key_;
    ExprContext ctx_;

public:
    Subscript(Expr *value, Expr *key, ExprContext ctx);
    ~Subscript();

    Expr *key() const;
    Expr *value() const;
    ExprContext ctx() const;
};


class Name : public Expr {
    std::string id_;
    ExprContext ctx_;

public:
    Name(const std::string &id, ExprContext ctx);

    const std::string &id() const;
    ExprContext ctx() const;
};


class List : public Expr {
    std::span<Expr *> elts_;
    ExprContext ctx_;

public:
    List(std::span<Expr *> elts, ExprContext ctx);
    ~List();

    const std::span<Expr *> &elts() const;
    ExprContext ctx() const;
};


class Tuple : public Expr {
    std::span<Expr *> elts_;
    ExprContext ctx_;

public:
    Tuple(std::span<Expr *> elts, ExprContext ctx);
    ~Tuple();

    const std::span<Expr *> &elts() const;
    ExprContext ctx() const;
};


class Stmt : public Node {};

class FunctionDef : public Stmt {
    std::string name_;
    std::span<std::string> args_;
    std::span<Stmt *> body_;
    Expr *returns_; // just nullptr if nothing

public:
    FunctionDef(const std::string &name, std::span<std::string> args, std::span<Stmt *> body);
    FunctionDef(const std::string &name, std::span<std::string> args, std::span<Stmt *> body, Expr *returns);
    ~FunctionDef();

    const std::string &name() const;
    const std::span<std::string> &args() const;
    const std::span<Stmt *> &body() const;
    Expr *returns() const;
    bool returns_anything() const;
};

class ClassDef : public Stmt {
    std::string name_;
    std::span<Stmt *> body_;

public:
    ClassDef(const std::string &name, std::span<Stmt *> body);
    ~ClassDef();

    const std::string &name() const;
    const std::span<Stmt *> body() const;
};

class Return : public Stmt {
    Expr *value_; // nullptr if returns nothing

public:
    Return(Expr *value);
    ~Return();

    bool returns_anything() const;
    Expr *value() const;
};

class Assign : public Stmt {
    std::span<Expr *> targets_;
    Expr *value_;

public:
    Assign(std::span<Expr *> targets, Expr *value);
    ~Assign();

    const std::span<Expr *> targets() const;
    Expr *value() const;
};

class AugAssign : public Stmt {
    Expr *target_;
    OperatorKind op_;
    Expr *value_;

public:
    AugAssign(Expr *target, OperatorKind op, Expr *value);
    ~AugAssign();

    Expr *target() const;
    OperatorKind op() const;
    Expr *value() const;
};

class While : public Stmt {
    Expr *test_;
    std::span<Stmt *> body_;

public:
    While(Expr *test, std::span<Stmt *> body);
    ~While();

    Expr *test() const;
    const std::span<Stmt *> &body() const;
};

class If : public Stmt {
    Expr *test_;
    std::span<Stmt *> body_;
    std::span<Stmt *> orelse_;

public:
    If(Expr *test, std::span<Stmt *> body, std::span<Stmt *> orelse);
    ~If();

    Expr *test() const;
    const std::span<Stmt *> &body() const;
    const std::span<Stmt *> &orelse() const;
};

class ExprStmt : public Stmt {
    Expr *value_;

public:
    ExprStmt(Expr *value);
    ~ExprStmt();

    Expr *value() const;
};

class Pass : public Stmt {};
class Break : public Stmt {};
class Continue : public Stmt {};

} // namespace ast
} // namespace yapvm