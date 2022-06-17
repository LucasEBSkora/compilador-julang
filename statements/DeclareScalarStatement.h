#pragma once

#include <vector>

#include "statement.h"
#include "expression.h"

namespace Julang
{
  struct DeclareScalarStatement : public Statement
  {

    DeclareScalarStatement(std::string identifier, Expression* initialValue = nullptr);
    virtual const std::string toString() const;

    ~DeclareScalarStatement();
    std::string identifier;
    Expression* initialValue;
  };

} // namespace Julang
