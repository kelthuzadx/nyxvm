#ifndef NYX_BYTECODE_H
#define NYX_BYTECODE_H

#include "../object/NObject.h"
#include "../runtime/Global.h"
#include <string>
#include <unordered_map>
#include <vector>

//===----------------------------------------------------------------------===//
// Representation of NyxVM bytecode
//===----------------------------------------------------------------------===//
struct Bytecode;

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
        NObject* inactive;
        NObject** active;
    } value{};

    ~FreeVar() = default;
};

struct Bytecode {
    int id;

    std::string funcName;

    std::vector<std::string> strings;

    std::unordered_map<std::string, int> localVars;

    std::vector<FreeVar*> freeVars;

    std::unordered_map<int, Bytecode*> callables;

    Bytecode* parent;

    int codeSize;

    nyx::int32* code;

    static const char*((builtin[])[2]);

    static int findBuiltinIndex(const std::string& name);

    explicit Bytecode(int id, const std::string& name);

    ~Bytecode();
};

#endif // NYX_BYTECODE_H
