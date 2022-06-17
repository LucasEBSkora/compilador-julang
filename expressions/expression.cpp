#include "expression.h"

#include <sstream>

namespace Julang
{

  ScalarReference::ScalarReference(std::string identifier) : identifier{identifier} {}

  const std::string ScalarReference::toString() const
  {
    return identifier;
  }

  IntegerLiteral::IntegerLiteral(int literal) : literal{literal} {}

  const std::string IntegerLiteral::toString() const
  {
    std::stringstream ss;
    ss << literal;
    return ss.str();
  }

  ArrayReference::ArrayReference(std::string identifier, Expression *index) : identifier{identifier}, index{index} {}

  ArrayReference::~ArrayReference()
  {
    delete index;
  }

  const std::string ArrayReference::toString() const
  {
    return identifier + "[" + index->toString() + "]";
  }

    std::string operatorToString(Operator op)
  {
    switch (op)
    {
    case EQ:
      return "==";
    case NE:
      return "!=";
    case LT:
      return "<";
    case GT:
      return ">";
    case LE:
      return "<=";
    case GE:
      return ">=";
    case SUM:
      return "+";
    case SUB:
      return "-";
    case ASSIGN:
      return "=";

    default:
      return "OP";
    }
  }

  BinaryOperation::BinaryOperation(Expression *lhs, Operator op, Expression *rhs) : lhs{lhs}, op{op}, rhs{rhs} {}

  BinaryOperation::~BinaryOperation()
  {
    delete lhs;
    delete rhs;
  }

  const std::string BinaryOperation::toString() const
  {
    return '(' + lhs->toString() + ' ' + rhs->toString() + ' ' +  ')';
  }



} // namespace Julang
