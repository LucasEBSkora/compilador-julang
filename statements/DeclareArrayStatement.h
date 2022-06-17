#pragma once

#include <vector>

#include "statement.h"
#include "expression.h"

namespace Julang
{
  struct DeclareArrayStatement : public Statement
  {

    DeclareArrayStatement(std::string identifier, unsigned int size);
    virtual const std::string toString() const;

    std::string identifier;
    unsigned int size;
  };

} // namespace Julang
