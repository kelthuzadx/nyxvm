#include "Bytecode.h"
#include "../runtime/Builtin.h"

const char*((Bytecode::builtin[])[2]) = {
    {"nyxffi_println", (const char*)nyxffi_println},
    {"nyxffi_print", (const char*)nyxffi_print},
    {"nyxffi_typeof", (const char*)nyxffi_typeof},
    {"nyxffi_len", (const char*)nyxffi_len},
    {"nyxffi_exit", (const char*)nyxffi_exit},
    {"nyxffi_assert", (const char*)nyxffi_assert},
    {"nyxffi_range", (const char*)nyxffi_range}};

Bytecode::~Bytecode() {
    delete[] code;
    for (auto& val : callables) {
        delete val.second;
    }
}

Bytecode::Bytecode(int id, const std::string& name) {
    this->id = id;
    code = new nyx::int32[65535];
    this->codeSize = -1;
    this->funcName = name;
    this->parent = nullptr;
}

int Bytecode::findBuiltinIndex(const std::string& name) {
    std::string target = "nyxffi_" + name;
    for (int i = 0; i < sizeof(builtin) / sizeof(builtin[0]); i++) {
        const char* builtinFuncName = builtin[i][0];
        if (target == builtinFuncName) {
            return i;
        }
    }
    return -1;
}