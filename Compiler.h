#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <string>

class Compiler {

    public:

        Compiler(std::string outputPath);
        virtual ~Compiler();

        //return 0 on success, 1 on failure
        virtual int generateCode() = 0;

    protected:
        std::string outputPath;

};

#endif
