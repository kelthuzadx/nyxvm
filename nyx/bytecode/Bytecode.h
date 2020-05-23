#ifndef NYX_BYTECODE_H
#define NYX_BYTECODE_H

#include "../runtime/Global.h"
#include "../runtime/Object.h"
#include <string>
#include <unordered_map>
#include <vector>

struct Bytecode;
//===----------------------------------------------------------------------===//
// Representation of NyxVM bytecode
//===----------------------------------------------------------------------===//
struct FreeVar {
    std::string name;
    bool isEnclosing{};
    FreeVar* endpoint{};
    int varIndex{};
    // Active state means free variable is still active in enclosing context, so
    // current context can only borrows the value. In contrast, inactive means
    // enclosing context is no longer live, current context owns the free
    // variable
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
