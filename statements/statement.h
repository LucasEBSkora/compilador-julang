#pragma once

#include <string>

namespace Julang
{
  struct Statement {
      virtual const std::string toString() const = 0;
      virtual ~Statement() {}
  };
  
} // namespace Julang
