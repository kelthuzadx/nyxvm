#include "Bytecode.h"

Bytecode::~Bytecode() {
    delete[] code;
    for (auto &val:functions) {
        delete val.second;
    }
}

Bytecode::Bytecode() {
    code = new nyx::int32[65535];
    this->codeSize = -1;
    this->funcName = "";
    this->parent = nullptr;
}

