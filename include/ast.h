#pragma once


namespace yapvm {
namespace ast {

class CmpOp {};

class Eq : CmpOp {};
class NotEq : CmpOp {};
class Lt : CmpOp {};
class LtE : CmpOp {};
class Gt : CmpOp {};
class GtE : CmpOp {};


class UnaryOp {};

class Invert : UnaryOp {};
class Not : UnaryOp {};

}
}