#ifndef NYX_BYTECODE_H
#define NYX_BYTECODE_H

#include <vector>
#include <string>
#include <unordered_map>
#include "../runtime/Global.h"


struct Bytecode {
    std::string funcName;
    std::vector<std::string> strings;
    int localSize;
    int bytecodeSize;
    nyx::int32 *bytecodes;

    explicit Bytecode();

    ~Bytecode();

    std::unordered_map<std::string, Bytecode *> functions;
};


#endif //NYX_BYTECODE_H
