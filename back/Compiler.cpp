#include "Compiler.h"

#include <iostream>

namespace Julang
{
  Compiler::Compiler(std::string outputPath, std::vector<Statement *>& statements) : outputPath{outputPath}, statements{statements} {}

  Compiler::~Compiler() {}

} // namespace Julang
