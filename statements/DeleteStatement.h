#pragma once

#include <vector>

#include "statement.h"
#include "expression.h"

namespace Julang
{
  struct DeleteStatement : public Statement
  {

    DeleteStatement(std::string identifier);
    virtual const std::string toString() const;

    std::string identifier;
  };

} // namespace Julang
