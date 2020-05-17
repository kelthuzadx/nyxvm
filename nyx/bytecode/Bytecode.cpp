#include "Bytecode.h"

Bytecode::~Bytecode() {
    delete[] bytecodes;
    for (auto &val:functions) {
        delete val.second;
    }
}

Bytecode::Bytecode() {
    bytecodes = new nyx::int32[65535];
}

