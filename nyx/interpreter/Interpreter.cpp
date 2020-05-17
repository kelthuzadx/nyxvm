#include "Interpreter.h"
#include "../bytecode/Opcode.h"
#include "../runtime/NyxVM.h"


//===----------------------------------------------------------------------===//
// Simple C++ bytecode interpreter
//===----------------------------------------------------------------------===//
Interpreter::Interpreter() {
    this->frame = nullptr;
}

Interpreter::~Interpreter() {
    frame = nullptr;
    for (auto &val:stack) {
        delete val;
    }
}


void Interpreter::neg(Object *object) {
    if (typeid(*object) == typeid(NInt)) {
        nyx::int32 val = -dynamic_cast<NInt *>(object)->value;
        frame->push(new NInt(val));
    } else if (typeid(*object) == typeid(NDouble)) {
        double val = -dynamic_cast<NDouble *>(object)->value;
        frame->push(new NDouble(val));
    } else {
        panic("should not reach here");
    }
}

void Interpreter::destroyFrame(){
    // Destroy current frame, current frame may points to next frame
    auto *temp = stack.back();
    stack.pop_back();
    if (!stack.empty()) {
        this->frame = stack.back();
    } else {
        this->frame = nullptr;
    }
    delete temp;
}

void Interpreter::execute(Bytecode *bytecode, int argc, Object **argv) {
    // Create execution frame
    this->frame = new Frame(bytecode->localSize);
    for (int i = argc - 1, k = 0; i >= 0; i--, k++) {
        frame->store(k, argv[i]);
    }
    stack.push_back(frame);

    // Set up bytecode size and max length
    int bytecodeSize = bytecode->bytecodeSize;
    auto *bytecodes = bytecode->bytecodes;

    // Execute bytecode
    {
        PhaseTime timer("execute bytecodes via simple c++ interpreter");
        for (int bci = 0; bci < bytecodeSize; bci++) {
            switch (bytecodes[bci]) {
                case CALL: {
                    int funcNameIndex = bytecodes[bci + 1];
                    int funcArgc = bytecodes[bci + 2];
                    std::string funcName = bytecode->strings[funcNameIndex];

                    auto **funcArgv = new Object *[funcArgc];
                    for (int k = 0; k < funcArgc; k++) {
                        funcArgv[k] = frame->pop();
                    }

                    const char *funcPtr = NyxVM::findBuiltin(funcName);
                    if (funcPtr != nullptr) {
                        ((void (*)(int, Object **)) funcPtr)(funcArgc, funcArgv);

                    } else {
                        if (auto iter = bytecode->functions.find(funcName);iter != bytecode->functions.cend()) {
                            Bytecode *userFunc = iter->second;
                            this->execute(userFunc, funcArgc, funcArgv);
                        }
                    }

                    // TODO release funcArgv
                    bci += 2;
                    break;
                }
                case CONST_I: {
                    nyx::int32 value = *(nyx::int32 *) (bytecodes + bci + 1);
                    auto *object = new NInt(value);
                    frame->push(object);
                    bci += 4;
                    break;
                }
                case CONST_D: {
                    double value = *(double *) (bytecodes + bci + 1);
                    auto *object = new NDouble(value);
                    frame->push(object);
                    bci += 8;
                    break;
                }
                case CONST_NULL: {
                    frame->push(nullptr);
                    break;
                }
                case CONST_STR: {
                    int index = bytecodes[bci + 1];
                    auto &str = bytecode->strings[index];
                    Object *object = new NString(str);
                    frame->push(object);
                    bci++;
                    break;
                }
                case ADD: {
                    Object *object2 = frame->pop();
                    Object *object1 = frame->pop();
                    arithmetic<ADD>(object1, object2);
                    break;
                }
                case SUB: {
                    Object *object2 = frame->pop();
                    Object *object1 = frame->pop();
                    arithmetic<SUB>(object1, object2);
                    break;
                }
                case MUL: {
                    Object *object2 = frame->pop();
                    Object *object1 = frame->pop();
                    arithmetic<MUL>(object1, object2);
                    break;
                }
                case DIV: {
                    Object *object2 = frame->pop();
                    Object *object1 = frame->pop();
                    arithmetic<DIV>(object1, object2);
                    break;
                }
                case REM: {
                    Object *object2 = frame->pop();
                    Object *object1 = frame->pop();
                    arithmetic<REM>(object1, object2);
                    break;
                }
                case TEST_EQ: {
                    Object *object2 = frame->pop();
                    Object *object1 = frame->pop();
                    compare<TEST_EQ>(object1, object2);
                    break;
                }
                case TEST_NE: {
                    Object *object2 = frame->pop();
                    Object *object1 = frame->pop();
                    compare<TEST_NE>(object1, object2);
                    break;
                }
                case TEST_GE: {
                    Object *object2 = frame->pop();
                    Object *object1 = frame->pop();
                    compare<TEST_GE>(object1, object2);
                    break;
                }
                case TEST_GT: {
                    Object *object2 = frame->pop();
                    Object *object1 = frame->pop();
                    compare<TEST_GT>(object1, object2);
                    break;
                }
                case TEST_LE: {
                    Object *object2 = frame->pop();
                    Object *object1 = frame->pop();
                    compare<TEST_LE>(object1, object2);
                    break;
                }
                case TEST_LT: {
                    Object *object2 = frame->pop();
                    Object *object1 = frame->pop();
                    compare<TEST_LT>(object1, object2);
                    break;
                }
                case JMP: {
                    int target = bytecodes[bci + 1];
                    bci = target - 1;
                    break;
                }
                case JMP_NE: {
                    auto *cond = dynamic_cast<NInt *>(frame->pop());
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
                case JMP_EQ: {
                    auto *cond = dynamic_cast<NInt *>(frame->pop());
                    assert(cond->value == 0 || cond->value == 1);
                    if (cond->value == 1) {
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
                    Object *object2 = frame->pop();
                    Object *object1 = frame->pop();
                    bitop<AND>(object1, object2);
                    break;
                }
                case OR: {
                    Object *object2 = frame->pop();
                    Object *object1 = frame->pop();
                    bitop<OR>(object1, object2);
                    break;
                }
                case NOT: {
                    Object *object1 = frame->pop();
                    bitop<NOT>(object1, nullptr);
                    break;
                }
                case NEG: {
                    Object *object = frame->pop();
                    neg(object);
                    break;
                }
                case LOAD: {
                    int index = bytecodes[bci + 1];
                    frame->load(index);
                    bci++;
                    break;
                }
                case STORE: {
                    Object *value = frame->pop();
                    int index = bytecodes[bci + 1];
                    frame->store(index, value);
                    bci++;
                    break;
                }
                case LOAD_INDEX: {
                    auto *length = dynamic_cast<NInt *>(frame->pop());
                    auto *array = dynamic_cast<NArray *>(frame->pop());
                    auto *elem = array->array[length->value];
                    frame->push(elem);
                    break;
                }
                case STORE_INDEX: {
                    auto *elemIndex = dynamic_cast<NInt *>(frame->pop());
                    auto *object = frame->pop();
                    auto *array = dynamic_cast<NArray *>(frame->pop());
                    array->array[elemIndex->value] = object;
                    break;
                }
                case NEW_ARR: {
                    int length = bytecodes[bci + 1];
                    frame->push(new NArray(length));
                    bci++;
                    break;
                }
                case DUP: {
                    frame->dup();
                    break;
                }
                case RETURN:{
                    destroyFrame();
                    return;
                }
                case RETURN_VAL:{
                    auto *temp = stack.back();
                    stack.pop_back();
                    if (stack.empty()) {
                        panic("return from top context");
                    }
                    this->frame = stack.back();
                    this->frame->push(temp->pop());
                    delete temp;
                    return;
                }
                default:
                    panic("invalid bytecode %d", bytecodes[bci]);
            }
        }
    }
    destroyFrame();
}
