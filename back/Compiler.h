#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <string>
#include <vector>

#include "statement.h"

namespace Julang
{

    class Compiler
    {

    public:
        Compiler(std::string outputPath, std::vector<Statement *>& statements);
        virtual ~Compiler();

        // return 0 on success, 1 on failure
        virtual int generateCode() = 0;

    protected:
        std::string outputPath;
        std::vector<Statement *>& statements;
    };
} // namespace Julang

#endif
