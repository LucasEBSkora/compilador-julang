#include "Assembler.h"
#include <iostream>
#include "statements.h"

#include <bitset>
namespace Julang
{
  const unsigned int Assembler::RAM_SIZE{128};

  Assembler::Assembler(std::string outputPath, std::vector<Statement *> &statements)
      : Compiler(outputPath, statements), outputFile{outputPath}, instructionIndex{0}, addressToName(RAM_SIZE)
  {
    if (!outputFile.is_open())
    {
      std::cout << "FAILED TO OPEN OUTPUT FILE AT " << outputPath << std::endl;
    }

    addressToName.assign(RAM_SIZE, "");
  }

  Assembler::~Assembler()
  {
  }

  int Assembler::generateCode()
  {
    for (Statement *stmt : statements)
    {
      if (dynamic_cast<IfStatement *>(stmt) || dynamic_cast<WhileStatement *>(stmt))
        outputFile << "/*" << stmt->toString() << "*/\n";
      else
        outputFile << "//" << stmt->toString() << '\n';

      outputFile << generateStmt(stmt) << '\n';
    }

    outputFile << "which would be translated to:\n";

    for (auto pair : instructions)
    {

      outputFile << pair.first << " => " << pair.second << ",\n";
    }

    outputFile.close();
    return 0;
  }

  std::string Assembler::generateStmt(Statement *stmt)
  {
    if (stmt == nullptr)
    {
      return "";
    }

    AssignmentStatement *assignment = dynamic_cast<AssignmentStatement *>(stmt);
    if (assignment)
    {
      unsigned int address = find(assignment->identifier).first;
      Expression *value = assignment->value;
      Expression *index = assignment->index;

      std::stringstream assignment;
      if (index)
      {
        BinaryOperation bop = BinaryOperation(new IntegerLiteral(address), SUM, index);
        assignment << loadAccumulator(&bop);
      }
      else
      {
        assignment << generateLD(00, true, address);
      }
      assignment << generateLD(0b11, false, 0);

      assignment << loadAccumulator(value)
                 << generateLD(0b11, false, 1);
      return assignment.str();
    }

    DeclareScalarStatement *Scalardeclaration = dynamic_cast<DeclareScalarStatement *>(stmt);
    if (Scalardeclaration)
    {
      unsigned int address = declare(Scalardeclaration->identifier).first;
      Expression *initial = Scalardeclaration->initialValue;
      if (initial)
      {
        std::stringstream assignment;
        assignment << loadAccumulator(initial) << generateLD(01, 0, address);
        return assignment.str();
      }
      else
      {
        return "\n";
      }
    }

    IfStatement *ifStmt = dynamic_cast<IfStatement *>(stmt);
    if (ifStmt)
    {
      const std::string condition = loadAndCompare(ifStmt->condition);

      std::stringstream body;

      const unsigned int JumpInstructionAddress = instructionIndex++;
      for (Statement *s : *ifStmt->statements)
      {
        body << generateStmt(s);
      }
      const unsigned int jumpDestination = instructionIndex;

      std::string jumpInstruction = generateJump(ifStmt->condition, JumpInstructionAddress, jumpDestination - JumpInstructionAddress);

      return condition + jumpInstruction + body.str();
    }

    DeclareArrayStatement *ArrayDeclaration = dynamic_cast<DeclareArrayStatement *>(stmt);
    if (ArrayDeclaration)
    {
      declare(ArrayDeclaration->identifier, ArrayDeclaration->size);
      return "\n";
    }

    DeleteStatement *deletion = dynamic_cast<DeleteStatement *>(stmt);
    if (deletion)
    {
      deleteName(deletion->identifier);
      return "\n";
    }

    WhileStatement *whileStmt = dynamic_cast<WhileStatement *>(stmt);
    if (whileStmt)
    {
      const unsigned int startAddress = instructionIndex;
      const std::string condition = loadAndCompare(whileStmt->condition);

      std::stringstream body;

      const unsigned int JumpInstructionAddress = instructionIndex++;
      for (Statement *s : *whileStmt->statements)
      {
        body << generateStmt(s);
      }

      body << generateJP(startAddress);
      const unsigned int jumpDestination = instructionIndex;

      std::string jumpInstruction = generateJump(whileStmt->condition, JumpInstructionAddress, jumpDestination - JumpInstructionAddress);

      return condition + jumpInstruction + body.str();
    }

    std::cout << "unknown statement type: " << stmt->toString();
    return "";
  }

  std::string Assembler::loadAndCompare(Expression *expr)
  {
    IntegerLiteral *integerLiteral = dynamic_cast<IntegerLiteral *>(expr);
    if (integerLiteral)
    {
      // ifs with integer literals always result in JRF (literal == 0) or JRT (otherwise)
      return "";
    }

    if (dynamic_cast<ScalarReference *>(expr) || dynamic_cast<ArrayReference *>(expr))
    {
      // ifs with references without operators will be interpreted as JREQ (zero) - since loadAccumulator will create
      // a LD in these cases and LD instructions set the Z status register bit, no comparison is actually needed.
      return loadAccumulator(expr);
    }

    BinaryOperation *binaryOperation = dynamic_cast<BinaryOperation *>(expr);

    if (binaryOperation)
    {
      if (binaryOperation->op == SUM || binaryOperation->op == SUB || binaryOperation->op == ASSIGN)
      {
        std::cout << "Operator " << operatorToString(binaryOperation->op) << " not supported in comparisons!" << std::endl;
        return "";
      }

      std::stringstream ss;
      ss << loadAccumulator(binaryOperation->lhs);
      auto selSource = findSelAndSource(binaryOperation->rhs);

      ss << generateCP(selSource.first, selSource.second);
      return ss.str();
    }

    std::cout << "unknown Expression type for condition: " << expr->toString();
    return "";
  }

  std::string Assembler::generateJump(Expression *expr, unsigned int index, int offset)
  {
    IntegerLiteral *integerLiteral = dynamic_cast<IntegerLiteral *>(expr);
    if (integerLiteral)
    {
      if (integerLiteral->literal == 0)
      {
        return generateJRxx(0b1110, offset, index);
      }
      else
      {
        return generateJRxx(0b1111, offset, index);
      }
    }

    if (dynamic_cast<ScalarReference *>(expr) || dynamic_cast<ArrayReference *>(expr))
    {
      // ifs with references without operators will be interpreted as JREQ (zero) - since loadAccumulator will create
      // a LD in these cases and LD instructions set the Z status register bit, no comparison is actually needed.
      return generateJRxx(0b0001, offset, index);
    }

    BinaryOperation *binaryOperation = dynamic_cast<BinaryOperation *>(expr);

    if (binaryOperation)
    {
      if (binaryOperation->op == SUM || binaryOperation->op == SUB || binaryOperation->op == ASSIGN)
      {
        std::cout << "Operator " << operatorToString(binaryOperation->op) << " not supported in comparisons!" << std::endl;
        return "";
      }

      unsigned char xx;
      switch (binaryOperation->op)
      {
      case EQ:
        xx = 0b0100;
        break;
      case NE:
        xx = 0b0001;
        break;
      case LT:
        xx = 0b0111;
        break;
      case GT:
        xx = 0b1001;
        break;
      case LE:
        xx = 0b1000;
        break;
      case GE:
        xx = 0b1010;
        break;
      default:
        xx = 0b1110;
        break;
      }

      return generateJRxx(xx, offset, index);
    }

    std::cout << "unknown Expression type: " << expr->toString();
    return "";
  }

  std::string Assembler::loadAccumulator(Expression *expression)
  {
    // For simplicity, we assume that no operation more complex than a single addition, subtraction or array access is used.
    IntegerLiteral *integerLiteral = dynamic_cast<IntegerLiteral *>(expression);
    if (integerLiteral)
    {
      return generateLD(0b00, true, integerLiteral->literal);
    }

    ScalarReference *scalarReference = dynamic_cast<ScalarReference *>(expression);
    if (scalarReference)
    {
      return generateLD(0b01, true, find(scalarReference->identifier).first);
    }

    ArrayReference *arrayReference = dynamic_cast<ArrayReference *>(expression);
    if (arrayReference)
    {
      return loadAccumulator(arrayReference->index) + generateLD(10, false, 0) + generateLD(10, true, 1);
    }

    BinaryOperation *binaryOperation = dynamic_cast<BinaryOperation *>(expression);

    if (binaryOperation)
    {
      if (binaryOperation->op != SUM && binaryOperation->op != SUB)
      {
        std::cout << "Operator " << operatorToString(binaryOperation->op) << " not supported in assignments!" << std::endl;
        return "";
      }

      std::stringstream ss;
      ss << loadAccumulator(binaryOperation->lhs);
      auto selSource = findSelAndSource(binaryOperation->rhs);

      if (binaryOperation->op == SUM)
        ss << generateADD(selSource.first, selSource.second);
      else if (binaryOperation->op == SUB)
        ss << generateSUB(selSource.first, selSource.second);

      return ss.str();
    }

    std::cout << "unknown Expression type: " << expression->toString();
    return "";
  }

  std::pair<unsigned char, int> Assembler::findSelAndSource(Expression *expr)
  {
    IntegerLiteral *integerLiteral = dynamic_cast<IntegerLiteral *>(expr);
    if (integerLiteral)
    {
      return {00, integerLiteral->literal};
    }

    ScalarReference *scalarReference = dynamic_cast<ScalarReference *>(expr);
    if (scalarReference)
    {
      return {01, find(scalarReference->identifier).first};
    }

    ArrayReference *arrayReference = dynamic_cast<ArrayReference *>(expr);
    if (arrayReference)
    {
      std::cout << "Array references in right hand side not supported" << std::endl;
      return {0, 0};
    }

    BinaryOperation *binaryOperation = dynamic_cast<BinaryOperation *>(expr);

    if (binaryOperation)
    {
      std::cout << "Binary operations in right hand side of other binary operations not supported" << std::endl;
      return {0, 0};
    }

    std::cout << "unknown expression type: " << expr->toString();
    return {0, 0};
  }

  std::pair<unsigned int, unsigned int> Assembler::declare(std::string identifier, unsigned int size /*= 1*/)
  {
    if (nameToAddress.count(identifier) != 0)
    {
      std::cout << identifier << "already declared!" << std::endl;
      return {2 * RAM_SIZE, 2 * RAM_SIZE};
    }

    unsigned int address;
    unsigned int contiguousFree = 0;
    for (address = 0; address < RAM_SIZE; ++address)
    {
      if (addressToName[address] == "")
      {
        ++contiguousFree;
        if (contiguousFree >= size)
        {
          address -= (contiguousFree - 1);
          break;
        }
      }
      else
      {
        contiguousFree = 0;
      }
    }

    if (address >= RAM_SIZE)
    {
      std::cout << "not enough memory to declare " << identifier << "! Please delete some variables if possible." << std::endl;
      return {2 * RAM_SIZE, 2 * RAM_SIZE};
    }

    auto pair = std::pair<unsigned int, unsigned int>{address, address + size};
    nameToAddress.emplace(identifier, pair);
    for (unsigned int i = address; i < address + size; ++i)
    {
      addressToName[address] = identifier;
    }

    return pair;
  }

  std::pair<unsigned int, unsigned int> Assembler::find(std::string identifier)
  {
    if (nameToAddress.count(identifier) == 0)
    {
      std::cout << identifier << "not declared!" << std::endl;
      return {0, 0};
    }
    else
    {
      return nameToAddress.at(identifier);
    }
  }

  void Assembler::deleteName(std::string identifier)
  {
    if (nameToAddress.count(identifier) == 0)
    {
      std::cout << identifier << "not declared!" << std::endl;
      return;
    }
    else
    {
      auto iterator = nameToAddress.find(identifier);
      auto pair = iterator->second;
      nameToAddress.erase(iterator);

      for (unsigned int i = pair.first; i < pair.second; ++i)
        addressToName[i] = "";
    }
  }

  std::string Assembler::generateADD(unsigned char sel, int source /* = 0*/)
  {
    std::stringstream assembly;
    std::stringstream binary;

    binary << "B\"001_" << std::bitset<2>(sel) << '_' << std::bitset<10>(source) << '"';

    assembly << "ADD A,";
    switch (sel)
    {
    case 0:
      assembly << "#$" << source;
      break;
    case 1:
      assembly << "$" << source;
      break;
    case 2:
      assembly << "(X)";
      break;
    case 3:
      assembly << "(Y)";
      break;
    default:
      std::cout << "invalid option for ADD selector: " << sel << std::endl;
    }

    assembly << " //" << binary.str();
    instructions.emplace(instructionIndex++, binary.str());
    return assembly.str() + '\n';
  }

  std::string Assembler::generateSUB(unsigned char sel, int source /* = 0*/)
  {
    std::stringstream assembly;
    std::stringstream binary;

    binary << "B\"010_" << std::bitset<2>(sel) << '_' << std::bitset<10>(source) << '"';

    assembly << "SUB A,";
    switch (sel)
    {
    case 0:
      assembly << "#$" << source;
      break;
    case 1:
      assembly << "$" << source;
      break;
    case 2:
      assembly << "(X)";
      break;
    case 3:
      assembly << "(Y)";
      break;
    default:
      std::cout << "invalid option for SUB selector: " << sel << std::endl;
    }

    assembly << " //" << binary.str();
    instructions.emplace(instructionIndex++, binary.str());
    return assembly.str() + '\n';
  }

  std::string Assembler::generateCP(unsigned char sel, int source /* = 0*/)
  {
    std::stringstream assembly;
    std::stringstream binary;

    binary << "B\"111_" << std::bitset<2>(sel) << '_' << std::bitset<10>(source) << '"';

    assembly << "CP A,";
    switch (sel)
    {
    case 0:
      assembly << "#$" << source;
      break;
    case 1:
      assembly << "$" << source;
      break;
    case 2:
      assembly << "(X)";
      break;
    case 3:
      assembly << "(Y)";
      break;
    default:
      std::cout << "invalid option for CP selector: " << sel << std::endl;
    }

    assembly << " //" << binary.str();
    instructions.emplace(instructionIndex++, binary.str());
    return assembly.str() + '\n';
  }

  std::string Assembler::generateLD(unsigned char sel, bool accumulatorAsDestination, int data /* = 0*/)
  {
    std::stringstream assembly;
    std::stringstream binary;

    binary << "B\"011_" << std::bitset<2>(sel) << '_' << std::bitset<1>(accumulatorAsDestination) << '_' << std::bitset<9>(data) << '"';

    std::stringstream otherOperand;
    switch (sel)
    {
    case 0:
      otherOperand << "#$" << data;
      break;
    case 1:
      otherOperand << "$" << data;
      break;
    case 2:
      if (data)
        otherOperand << "(X)";
      else
        otherOperand << "X";
      break;
    case 3:
      if (data)
        otherOperand << "(Y)";
      else
        otherOperand << "Y";
      break;
    default:
      std::cout << "invalid option for LD selector: " << std::bitset<2>(sel) << std::endl;
    }

    if (accumulatorAsDestination)
    {
      assembly << "LD A," << otherOperand.str();
    }
    else
    {
      assembly << "LD " << otherOperand.str() << ",A";
    }

    assembly << " //" << binary.str();
    instructions.emplace(instructionIndex++, binary.str());
    return assembly.str() + '\n';
  }

  std::string Assembler::generateJRxx(unsigned char xx, int offset, unsigned int index)
  {
    std::stringstream assembly;
    std::stringstream binary;

    binary << "B\"101_" << std::bitset<4>(xx) << '_' << std::bitset<8>(offset) << '"';

    assembly << "JR";
    switch (xx)
    {
    case 0b0000:
      assembly << "ULT"; // could be JRC, but we always generate assembly as JRULT
      break;
    case 0b0001:
      assembly << "EQ";
      break;
    case 0b0010:
      assembly << "MI";
      break;
    case 0b0011:
      assembly << "UGE"; // could be JRNC, but we always generate assembly as JRUGE
      break;
    case 0b0100:
      assembly << "NE";
      break;
    case 0b0101:
      assembly << "NV";
      break;
    case 0b0110:
      assembly << "PL";
      break;
    case 0b0111:
      assembly << "SGE";
      break;
    case 0b1000:
      assembly << "SGT";
      break;
    case 0b1001:
      assembly << "SLE";
      break;
    case 0b1010:
      assembly << "SLT";
      break;
    case 0b1011:
      assembly << "UGT";
      break;
    case 0b1100:
      assembly << "ULE";
      break;
    case 0b1101:
      assembly << "V";
      break;
    case 0b1110:
      assembly << "T";
      break;
    case 0b1111:
      assembly << "F";
      break;
    default:
      std::cout << "invalid option for JRxx condition: " << xx << std::endl;
    }
    assembly << " $" << offset << " //" << binary.str();
    instructions.emplace(index, binary.str());
    return assembly.str() + '\n';
  }

  std::string Assembler::generateJP(unsigned int address) {
     std::stringstream assembly;
    std::stringstream binary;

    binary << "B\"101_" << std::bitset<12>(address)  << '"';

    assembly << "JP $" << address << " //" << binary.str();
    instructions.emplace(instructionIndex++, binary.str());
    return assembly.str() + '\n';
  }

} // namespace Julang