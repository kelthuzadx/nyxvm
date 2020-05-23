#include "Bytecode.h"

Bytecode::~Bytecode() {
    delete[] code;
    for (auto& val : functions) {
        delete val.second;
    }
}

Bytecode::Bytecode() {
    code = new nyx::int32[65535];
    this->codeSize = -1;
    this->funcName = "";
    this->parent = nullptr;
}
Bytecode* Bytecode::findLocalVar(const std::string& name) {
    if (parent == nullptr) {
        return nullptr;
    }
    Bytecode* temp = parent;
    while (temp != nullptr) {
        if (temp->localMap.find(name) != temp->localMap.end()) {
            return temp;
        }
        temp = temp->parent;
    }
    return nullptr;
}
