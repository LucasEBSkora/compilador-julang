#pragma once

#include <vector>

#include "statement.h"
#include "expression.h"

namespace Julang
{
  struct IfStatement : public Statement
  {

    IfStatement(Expression *condition, std::vector<Statement *>* statements);
    virtual const std::string toString() const;

    ~IfStatement();

    Expression *condition;
    std::vector<Statement *>* statements;
  };

} // namespace Julang
