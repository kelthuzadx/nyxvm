#include "Bytecode.h"

Bytecode::~Bytecode() {
    delete[] bytecodes;
    for (auto &val:functions) {
        delete val.second;
    }
}
