#pragma once

#include <string>
#include <iostream>
#include <variant>

namespace Julang
{

  struct Expression
  {
    virtual const std::string toString() const = 0;
    virtual ~Expression(){};
  };

  struct ScalarReference : Expression
  {
    ScalarReference(std::string identifier);
    virtual const std::string toString() const;

    std::string identifier;
  };

  struct IntegerLiteral : Expression
  {
    IntegerLiteral(int literal);
    virtual const std::string toString() const;

    int literal;
  };

  struct ArrayReference : Expression
  {
    ArrayReference(std::string identifier, Expression *index);
    ~ArrayReference();
    virtual const std::string toString() const;

    std::string identifier;
    Expression *index;
  };

  enum Operator
  {
    EQ,     // ==
    NE,     // !=
    LT,     // <
    GT,     // >
    LE,     // <=
    GE,     // >=
    SUM,    // +
    SUB,    // -
    ASSIGN, // =
  };

  std::string operatorToString(Operator op);

  struct BinaryOperation : Expression
  {
    BinaryOperation(Expression *lhs, Operator op, Expression *rhs);
    ~BinaryOperation();
    virtual const std::string toString() const;

    Expression *lhs;
    Operator op;
    Expression *rhs;
  };

} // namespace Julang
