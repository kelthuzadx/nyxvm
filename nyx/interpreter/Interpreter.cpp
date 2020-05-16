#include <iostream>
#include "Interpreter.h"
#include "../bytecode/Opcode.h"
#include "../runtime/Builtin.h"


static const char *(builtin[])[2] = {
        {"nyxffi_println", (char *) nyxffi_println},
};

Interpreter::Interpreter(MetaArea *meta) :
        meta(meta) {
}

void Interpreter::execute() {
    frame = new Frame;
    stack.push_back(frame);

    const int bytecodeSize = meta->bytecodeSize;
    const nyx::int8 *bytecodes = meta->bytecodes;
    int bci = 0;

    PhaseTime timer("execute bytecode");
    for (int i = 0; i < bytecodeSize; i++) {
        switch (bytecodes[i]) {
            case CALL: {
                int funcNameIndex = bytecodes[i + 1];
                int funcArgc = bytecodes[i + 2];
                std::string funcName = meta->strings[funcNameIndex];

                auto **argv = new Object *[funcArgc];
                for (int k = 0; k < funcArgc; k++) {
                    argv[k] = frame->slots.back();
                    frame->slots.pop_back();
                }

                const char *funcPtr = findBuiltin(funcName);
                if (funcPtr != nullptr) {
                    ((void (*)(int, Object **)) funcPtr)(funcArgc, argv);
                }
                i += 3;
                break;
            }
            case CONST_I: {
                nyx::int32 value = *(int *) (bytecodes + i + 1);
                auto *object = new NInt(value);
                frame->slots.push_back(object);
                i += 4;
                break;
            }
            case ADD: {
                Object *object1 = frame->slots.back();
                frame->slots.pop_back();
                Object *object2 = frame->slots.back();
                frame->slots.pop_back();
                if (typeid(*object1) == typeid(NInt)) {
                    NInt *o1 = dynamic_cast<NInt *>(object1);
                    if (typeid(*object2) == typeid(NInt)) {
                        NInt *o2 = dynamic_cast<NInt *>(object2);
                        NInt *res = new NInt(o1->value + o2->value);
                        frame->slots.push_back(res);
                    }
                }
                break;
            }
            default:
                panic("should not reach here");
        }
    }
}

const char *Interpreter::findBuiltin(const std::string &name) {
    std::string target = "nyxffi_" + name;
    for (int i = 0; i < sizeof(builtin) / sizeof(builtin[0]); i++) {
        const char *bname = builtin[i][0];
        if (target == bname) {
            return builtin[i][1];
        }
    }
    return nullptr;
}
