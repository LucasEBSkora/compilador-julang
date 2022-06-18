#include "WhileStatement.h"

#include <sstream>

namespace Julang
{
  WhileStatement::WhileStatement(Expression *condition, std::vector<Statement *> *statements)
      : condition{condition}, statements{statements}
  {
  }

  const std::string WhileStatement::toString() const
  {
    std::stringstream ss;
    ss << "[WHILE " << condition->toString() << " DO:\n";
    for (Statement *stmt : *statements)
      ss << '\t' << stmt->toString() << '\n';
    ss << "]";
    return ss.str();
  }

  WhileStatement::~WhileStatement()
  {
    delete condition;
    for (Statement *stmt : *statements)
    {
      delete stmt;
    }
    delete statements;
  }

} // namespace Julang
