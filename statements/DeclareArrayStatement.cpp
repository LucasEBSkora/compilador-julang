#include "DeclareArrayStatement.h"

#include <sstream>

namespace Julang
{
  DeclareArrayStatement::DeclareArrayStatement(std::string identifier, unsigned int size)
    : identifier{identifier}, size{size}
  {}
  
  const std::string DeclareArrayStatement::toString() const {
    std::stringstream ss;
    ss << "[DECLARE ARRAY \"" << identifier << "\" OF SIZE " << size << ']';
    return ss.str();
  }

} // namespace Julang
