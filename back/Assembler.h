#pragma once

#include "Compiler.h"

#include "expression.h"
#include "statements.h"

#include <fstream>
#include <sstream>
#include <map>
#include <vector>
namespace Julang
{

  class Assembler : public Compiler
  {
  public:
    Assembler(std::string outputPath, std::vector<Statement *>& statements);
    ~Assembler();
    virtual int generateCode() override;

  private:
    std::string generateStmt(Statement* stmt);
    std::string loadAccumulator(Expression* expr);
    std::string loadAndCompare(Expression* expr);
    std::string generateJump(Expression* expr, unsigned int index, int offset);
    std::pair<unsigned char, int> findSelAndSource(Expression* expr);


    std::pair<unsigned int, unsigned int> declare(std::string identifier, unsigned int size = 1);
    std::pair<unsigned int, unsigned int> find(std::string identifier);
    void deleteName(std::string identifier);

    std::string generateADD(unsigned char sel, int source = 0);
    std::string generateSUB(unsigned char sel, int source = 0);
    std::string generateCP(unsigned char sel, int source = 0);
    std::string generateLD(unsigned char sel, bool accumulatorAsDestination, int data = 0);
    std::string generateJRxx(unsigned char xx, int offset, unsigned int index);
    std::string generateJP(unsigned int address);    

    std::ofstream outputFile;
    std::map<unsigned int, std::string> instructions;
    unsigned int instructionIndex;
    static const unsigned int RAM_SIZE;
    std::map<std::string, std::pair<unsigned int, unsigned int>> nameToAddress;
    std::vector<std::string> addressToName;

  };
} // namespace Julang