/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 Krzysztof Narkiewicz <krzysztof.narkiewicz@ezaquarii.com>
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 */

#include <iostream>
#include "scanner.h"
#include "parser.hpp"
#include "interpreter.h"
#include "Assembler.h"

using namespace Julang;
using namespace std;

int main(int argc, char **argv) {

    if (argc != 3)
    {
        std::cerr << "Usage: julang <in-file> <out-file>\n"
                  << "where in-file  is source file to compile and out-file is the name of the assembly file" << std::endl;
        return -1;
    }

    Interpreter i{argv[1]};
    std::ifstream file(argv[1]);
    i.switchInputStream(&file);
    int res = i.parse();
    if (res == 0)
    {
        Assembler assembler{argv[2], i.getStmts()};
        res = assembler.generateCode();
    }

    return res;
}
