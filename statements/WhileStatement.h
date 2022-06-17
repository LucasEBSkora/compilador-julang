#pragma once

#include <vector>

#include "statement.h"
#include "expression.h"

namespace Julang
{
  struct WhileStatement : public Statement
  {

    WhileStatement(Expression *condition, std::vector<Statement *>* statements);
    virtual const std::string toString() const;
    ~WhileStatement();

    Expression *condition;
    std::vector<Statement *>* statements;
  };

} // namespace Julang
