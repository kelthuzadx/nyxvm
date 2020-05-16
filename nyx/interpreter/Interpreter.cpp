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

Object *Interpreter::pop() {
    Object *obj = frame->slots.back();
    frame->slots.pop_back();
    return obj;
}

void Interpreter::push(Object *obj) {
    frame->slots.push_back(obj);
}

void Interpreter::neg(Object *object) {
    if (typeid(*object) == typeid(NInt)) {
        nyx::int32 val = -dynamic_cast<NInt *>(object)->value;
        push(new NInt(val));
    } else if (typeid(*object) == typeid(NDouble)) {
        double val = -dynamic_cast<NDouble *>(object)->value;
        push(new NDouble(val));
    } else {
        panic("should not reach here");
    }
}

void Interpreter::execute() {
    frame = new Frame;
    stack.push_back(frame);

    int bytecodeSize = meta->bytecodeSize;
    auto *bytecodes = meta->bytecodes;

    PhaseTime timer("execute bytecode");
    for (int bci = 0; bci < bytecodeSize; bci++) {
        switch (bytecodes[bci]) {
            case CALL: {
                int funcNameIndex = bytecodes[bci + 1];
                int funcArgc = bytecodes[bci + 2];
                std::string funcName = meta->strings[funcNameIndex];

                auto **argv = new Object *[funcArgc];
                for (int k = 0; k < funcArgc; k++) {
                    argv[k] = pop();
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
                push(object);
                bci += 4;
                break;
            }
            case CONST_D: {
                double value = *(double *) (bytecodes + bci + 1);
                auto *object = new NDouble(value);
                push(object);
                bci += 8;
                break;
            }
            case CONST_NULL:{
                push(nullptr);
                break;
            }
            case ADD: {
                Object *object1 = pop();
                Object *object2 = pop();
                arithmetic<ADD>(object1, object2);
                break;
            }
            case SUB: {
                Object *object1 = pop();
                Object *object2 = pop();
                arithmetic<SUB>(object1, object2);
                break;
            }
            case MUL: {
                Object *object1 = pop();
                Object *object2 = pop();
                arithmetic<MUL>(object1, object2);
                break;
            }
            case DIV: {
                Object *object1 = pop();
                Object *object2 = pop();
                arithmetic<DIV>(object1, object2);
                break;
            }
            case REM: {
                Object *object1 = pop();
                Object *object2 = pop();
                arithmetic<REM>(object1, object2);
                break;
            }
            case TEST: {
                Object *object1 = pop();
                compare<TEST>(object1, nullptr);
                break;
            }
            case TEST_EQ: {
                Object *object1 = pop();
                Object *object2 = pop();
                compare<TEST_EQ>(object1, object2);
                break;
            }
            case TEST_NE: {
                Object *object1 = pop();
                Object *object2 = pop();
                compare<TEST_NE>(object1, object2);
                break;
            }
            case TEST_GE: {
                Object *object1 = pop();
                Object *object2 = pop();
                compare<TEST_GE>(object1, object2);
                break;
            }
            case TEST_GT: {
                Object *object1 = pop();
                Object *object2 = pop();
                compare<TEST_GT>(object1, object2);
                break;
            }
            case TEST_LE: {
                Object *object1 = pop();
                Object *object2 = pop();
                compare<TEST_LE>(object1, object2);
                break;
            }
            case TEST_LT: {
                Object *object1 = pop();
                Object *object2 = pop();
                compare<TEST_LT>(object1, object2);
                break;
            }
            case JMP: {
                int target = bytecodes[bci + 1];
                bci = target - 1;
                break;
            }
            case JMP_NE: {
                auto *cond = dynamic_cast<NInt *>(pop());
                assert(cond->value == 0 || cond->value == 1);
                if (cond->value != 0) {
                    // Goto target
                    int target = bytecodes[bci + 1];
                    bci = target - 1;
                } else {
                    // Otherwise, just forward bci
                    bci++;
                }
                break;
            }
            case JMP_EQ: {
                auto *cond = dynamic_cast<NInt *>(pop());
                assert(cond->value == 0 || cond->value == 1);
                if (cond->value == 0) {
                    // Goto target
                    int target = bytecodes[bci + 1];
                    bci = target - 1;
                } else {
                    // Otherwise, just forward bci
                    bci++;
                }
                break;
            }
            case AND: {
                Object *object1 = pop();
                Object *object2 = pop();
                bitop<AND>(object1, object2);
                break;
            }
            case OR: {
                Object *object1 = pop();
                Object *object2 = pop();
                bitop<OR>(object1, object2);
                break;
            }
            case NOT: {
                Object *object1 = pop();
                bitop<NOT>(object1, nullptr);
                break;
            }
            case NEG: {
                Object *object = pop();
                neg(object);
                break;
            }
            default:
                panic("invalid bytecode %d", bytecodes[bci]);
        }
    }
}

