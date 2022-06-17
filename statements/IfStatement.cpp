#include "IfStatement.h"

#include <sstream>

namespace Julang
{
  IfStatement::IfStatement(Expression *condition, std::vector<Statement *>* statements) : condition{condition}, statements{statements}
  {}

  const std::string IfStatement::toString() const {
    std::stringstream ss;
    ss << "[IF" << condition->toString() << "THEN:\n";
    for (Statement* stmt : *statements) 
      ss << '\t' << stmt->toString() << '\n';
    ss << "]";
    return ss.str();
  }


  IfStatement::~IfStatement() {
    delete condition;
    for (Statement* stmt : *statements) {
      delete stmt;
    }
    delete statements;
  }
} // namespace Julang
