#include "DeleteStatement.h"

#include <sstream>

namespace Julang
{
  DeleteStatement::DeleteStatement(std::string identifier)
    : identifier{identifier}
  {}
  
  const std::string DeleteStatement::toString() const {
    return "[DELETE " + identifier + "]";
  }

} // namespace Julang
