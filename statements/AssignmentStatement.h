#pragma once

#include <vector>

#include "statement.h"
#include "expression.h"

namespace Julang
{
  struct AssignmentStatement : public Statement
  {

    AssignmentStatement(std::string identifier, Expression* value, Expression* index = nullptr);
    virtual const std::string toString() const;

    ~AssignmentStatement();

    std::string identifier;
    Expression* value;
    Expression* index;
  };

} // namespace Julang
