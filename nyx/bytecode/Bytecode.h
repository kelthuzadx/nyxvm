#ifndef NYX_BYTECODE_H
#define NYX_BYTECODE_H

#include <vector>
#include <string>
#include <unordered_map>
#include "../runtime/Global.h"


struct Bytecode {
    std::vector<std::string> strings;

    int localSize;
    int bytecodeSize;
    nyx::int32 *bytecodes;

    std::unordered_map<std::string, Bytecode *> functions;

    ~Bytecode();
};


#endif //NYX_BYTECODE_H
