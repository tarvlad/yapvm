#pragma once


#include <span>


namespace yapvm {
namespace ast {


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

}
}