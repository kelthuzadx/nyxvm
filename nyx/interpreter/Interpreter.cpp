#include <iostream>
#include "Interpreter.h"
#include "../bytecode/Opcode.h"
#include "../runtime/NyxVM.h"


//===----------------------------------------------------------------------===//
// Simple C++ bytecode interpreter
//===----------------------------------------------------------------------===//
Interpreter::Interpreter(MetaArea *meta) :
        meta(meta) {
}

void Interpreter::execute() {
    frame = new Frame;
    stack.push_back(frame);

    const int bytecodeSize = meta->bytecodeSize;
    const nyx::int8 *bytecodes = meta->bytecodes;

    PhaseTime timer("execute bytecode");
    for (int bci = 0; bci < bytecodeSize; bci++) {
        switch (bytecodes[bci]) {
            case CALL: {
                int funcNameIndex = bytecodes[bci + 1];
                int funcArgc = bytecodes[bci + 2];
                std::string funcName = meta->strings[funcNameIndex];

                auto **argv = new Object *[funcArgc];
                for (int k = 0; k < funcArgc; k++) {
                    argv[k] = frame->slots.back();
                    frame->slots.pop_back();
                }

                const char *funcPtr = NyxVM::findBuiltin(funcName);
                if (funcPtr != nullptr) {
                    ((void (*)(int, Object **)) funcPtr)(funcArgc, argv);
                }
                bci += 2;
                break;
            }
            case CONST_I: {
                nyx::int32 value = *(nyx::int32 *) (bytecodes + bci + 1);
                auto *object = new NInt(value);
                frame->slots.push_back(object);
                bci += 4;
                break;
            }
            case CONST_D: {
                double value = *(double *) (bytecodes + bci + 1);
                auto *object = new NDouble(value);
                frame->slots.push_back(object);
                bci += 8;
                break;
            }
            case ADD: {
                Object *object1 = frame->slots.back();
                frame->slots.pop_back();
                Object *object2 = frame->slots.back();
                frame->slots.pop_back();
                arithmetic<ADD>(object1, object2);
                break;
            }
            case SUB: {
                Object *object1 = frame->slots.back();
                frame->slots.pop_back();
                Object *object2 = frame->slots.back();
                frame->slots.pop_back();
                arithmetic<SUB>(object1, object2);
                break;
            }
            case MUL: {
                Object *object1 = frame->slots.back();
                frame->slots.pop_back();
                Object *object2 = frame->slots.back();
                frame->slots.pop_back();
                arithmetic<MUL>(object1, object2);
                break;
            }
            case DIV: {
                Object *object1 = frame->slots.back();
                frame->slots.pop_back();
                Object *object2 = frame->slots.back();
                frame->slots.pop_back();
                arithmetic<DIV>(object1, object2);
                break;
            }
            case REM: {
                Object *object1 = frame->slots.back();
                frame->slots.pop_back();
                Object *object2 = frame->slots.back();
                frame->slots.pop_back();
                arithmetic<REM>(object1, object2);
                break;
            }
            default:
                panic("invalid bytecode %d", bytecodes[bci]);
        }
    }
}
