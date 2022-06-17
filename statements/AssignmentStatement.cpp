#include "AssignmentStatement.h"

#include <sstream>

namespace Julang
{
  AssignmentStatement::AssignmentStatement(std::string identifier, Expression *value, Expression *index /*= nullptr*/)
      : identifier{identifier}, value{value}, index{index}
  {
  }

  AssignmentStatement::~AssignmentStatement()
  {
    delete value;
    if (index)
      delete index;
  }

  const std::string AssignmentStatement::toString() const
  {
    return "[ASSIGN " + identifier + (index != nullptr ? (" AT INDEX " + index->toString()) : "") + " TO VALUE " + value->toString() + "]";
  }

} // namespace Julang
