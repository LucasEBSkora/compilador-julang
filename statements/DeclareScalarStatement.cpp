#include "DeclareScalarStatement.h"

#include <sstream>

namespace Julang
{
  DeclareScalarStatement::DeclareScalarStatement(std::string identifier, Expression* initialValue/* = nullptr*/)
    : identifier{identifier}, initialValue{initialValue}
  {}
  
  const std::string DeclareScalarStatement::toString() const {
    return "[DECLARE " + identifier + (initialValue != nullptr ? (" WITH INITIAL VALUE " + initialValue->toString() ) : "") + "]";
  }

  DeclareScalarStatement::~DeclareScalarStatement() {
    if (initialValue) delete initialValue;
  }

} // namespace Julang
