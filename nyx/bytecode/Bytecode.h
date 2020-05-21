#ifndef NYX_BYTECODE_H
#define NYX_BYTECODE_H

#include "../runtime/Global.h"
#include "../runtime/Object.h"
#include <string>
#include <unordered_map>
#include <vector>

struct Bytecode;

struct FreeVar {
    std::string name;
    bool isEnclosing{};
    FreeVar* endpoint{};
    int varIndex{};
    union {
        Object* inactive;
        Object** active;
    } value{};

    ~FreeVar() = default;
};

struct Bytecode {
    std::string funcName;
    std::vector<std::string> strings;
    std::unordered_map<std::string, int> localMap;

    std::vector<FreeVar*> freeVars;

    int codeSize;
    nyx::int32* code;

    explicit Bytecode();

    ~Bytecode();

    std::unordered_map<std::string, Bytecode*> functions;
    std::unordered_map<int, Bytecode*> closures;
    Bytecode* parent;
};

#endif // NYX_BYTECODE_H
