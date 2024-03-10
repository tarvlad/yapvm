#pragma once


namespace yapvm {
namespace ast {

enum CmpOpKind {
    EQ, NOT_EQ, LT, LTE, GT, GTE
};


class CmpOp {
public:
    virtual CmpOpKind kind() const = 0;
};

class Eq : CmpOp {
public:
    CmpOpKind kind() const;
};
class NotEq : CmpOp {
public:
    CmpOpKind kind() const;
};
class Lt : CmpOp {
public:
    CmpOpKind kind() const;
};
class LtE : CmpOp {
public:
    CmpOpKind kind() const;
};
class Gt : CmpOp {
public:
    CmpOpKind kind() const;
};
class GtE : CmpOp {
public:
    CmpOpKind kind() const;
};


//TODO
class UnaryOp {};

class Invert : UnaryOp {};
class Not : UnaryOp {};


class Operator {};

class Add : Operator {};
class Sub : Operator {};
class Mult : Operator {};
class Div : Operator {};
class Mod : Operator {};
class Pow : Operator {};
class LShift : Operator {};
class RShift : Operator {};
class BitOr : Operator {};
class BitXor : Operator {};
class BitAnd : Operator {};
class FloorDiv : Operator {};


class BoolOp {};

class And : BoolOp {};
class Or : BoolOp {};


class ExprContext {};

class Load : ExprContext {};
class Store : ExprContext {};
class Del : ExprContext {};


class Expression {};

}
}