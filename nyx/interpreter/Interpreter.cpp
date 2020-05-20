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

void Interpreter::createFrame(Bytecode *bytecode, int argc, Object **argv) {
    // Create execution frame
    this->frame = new Frame(bytecode->localMap.size());
    for (int i = argc - 1, k = 0; i >= 0; i--, k++) {
        auto *obj = argv[i];
        frame->store(k, obj);
    }

    if(!bytecode->freeVars.empty()){
        for(auto* freeVar: bytecode->freeVars){
            if(freeVar->isEnclosing){
                freeVar->value = &frame->local()[freeVar->varIndex];
            }
        }
    }

    stack.push_back(frame);
}

void Interpreter::destroyFrame(Bytecode *bytecode,bool hasReturnValue) {
    // Destroy current frame, current frame may points to next frame
    auto *temp = stack.back();
    Object* returnVale = nullptr;
    if(hasReturnValue){
        returnVale = temp->pop();
    }
    stack.pop_back();
    if (!stack.empty()) {
        this->frame = stack.back();
        if(returnVale!= nullptr){
            this->frame->push(returnVale);
        }
    } else {
        this->frame = nullptr;
    }

    if(!bytecode->freeVars.empty()){
        for(auto* freeVar: bytecode->freeVars){
            if(freeVar->isEnclosing){
                // "Move" free variable to endpoint
                freeVar->endpoint->value = freeVar->value;
                temp->local()[freeVar->varIndex] = nullptr;
            }
        }
    }
   // delete temp;
}

void Interpreter::call(Bytecode* bytecode, int bci) {
    int funcArgc = bytecode->code[bci + 1];

    auto **funcArgv = new Object *[funcArgc];
    for (int k = funcArgc - 1; k >= 0; k--) {
        funcArgv[k] = frame->pop();
    }

    Object *callee = frame->pop();
    if (typeid(*callee) == typeid(NString)) {
        auto funcName = dynamic_cast<NString *>(callee)->value;
        const char *funcPtr = NyxVM::findBuiltin(funcName);
        if (funcPtr != nullptr) {
            Object *result = ((Object *(*)(int, Object **)) funcPtr)(funcArgc, funcArgv);
            frame->push(result);
        } else {
            if (auto iter = bytecode->functions.find(funcName);iter != bytecode->functions.cend()) {
                Bytecode *userFunc = iter->second;
                this->execute(userFunc, funcArgc, funcArgv);
            }else{
                int closureIndex = bytecode->localMap[funcName];
                if(typeid(*frame->local()[closureIndex]) == typeid(NClosure)){
                    auto* closureObject = dynamic_cast<NClosure*>(frame->local()[closureIndex]);
                    this->execute(closureObject->code, funcArgc, funcArgv);
                }

            }

        }
    }

    // TODO release funcArgv
}

void Interpreter::execute(Bytecode *bytecode, int argc, Object **argv) {
    createFrame(bytecode, argc, argv);
    auto *code = bytecode->code;
    // Execute bytecode
    {
        for (int bci = 0; bci < bytecode->codeSize; bci++) {
            switch (code[bci]) {
                case CALL: {
                    call(bytecode, bci);
                    bci += 1;
                    break;
                }
                case CONST_I: {
                    nyx::int32 value = *(nyx::int32 *) (code + bci + 1);
                    auto *object = new NInt(value);
                    frame->push(object);
                    bci += 4;
                    break;
                }
                case CONST_D: {
                    double value = *(double *) (code + bci + 1);
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
                    int index = code[bci + 1];
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
                    int target = code[bci + 1];
                    bci = target - 1;
                    break;
                }
                case JMP_NE: {
                    auto *cond = dynamic_cast<NInt *>(frame->pop());
                    assert(cond->value == 0 || cond->value == 1);
                    if (cond->value == 0) {
                        // Goto target
                        int target = code[bci + 1];
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
                        int target = code[bci + 1];
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
                    int index = code[bci + 1];
                    frame->load(index);
                    bci++;
                    break;
                }
                case STORE: {
                    Object *value = frame->pop();
                    int index = code[bci + 1];
                    frame->store(index, value);
                    bci++;
                    break;
                }
                case LOAD_INDEX: {
                    auto *index = dynamic_cast<NInt *>(frame->pop());
                    auto *array = dynamic_cast<NArray *>(frame->pop());
                    auto *elem = array->array[index->value];
                    frame->push(elem);
                    break;
                }
                case STORE_INDEX: {
                    auto *index = dynamic_cast<NInt *>(frame->pop());
                    auto *object = frame->pop();
                    auto *array = dynamic_cast<NArray *>(frame->pop());
                    array->array[index->value] = object;
                    break;
                }
                case NEW_ARR: {
                    int length = code[bci + 1];
                    frame->push(new NArray(length));
                    bci++;
                    break;
                }
                case DUP: {
                    frame->dup();
                    break;
                }
                case RETURN: {
                    destroyFrame(bytecode,false);
                    return;
                }
                case RETURN_VAL: {
                    destroyFrame(bytecode, true);
                    return;
                }
                case ARR_LEN: {
                    auto *array = frame->pop();
                    if (typeid(*array) != typeid(NArray)) {
                        panic("array length is only operated on Array type");
                    }
                    int length = dynamic_cast<NArray *>(array)->length;
                    frame->push(new NInt(length));
                    break;
                }
                case CONST_CLOSURE:{
                    int closureIndex = code[bci+1];
                    frame->push(new NClosure(bytecode->closures[closureIndex]));
                    bci++;
                    break;
                }
                case LOAD_FREE: {
                    int freeIndex = code[bci+1];
                    FreeVar* freeVar = bytecode->freeVars[freeIndex];
                    if(*(freeVar->value)!=nullptr){
                        Object* obj = *(freeVar->value);
                        frame->push(*(freeVar->value));
                    }else{
                        frame->push(*(freeVar->endpoint->value));
                    }
                    bci++;
                    break;
                }
                default:
                    panic("invalid bytecode %d", code[bci]);
            }
        }
    }
    destroyFrame(bytecode, false);
}

void Interpreter::execute(Bytecode *bytecode) {
    PhaseTime timer("execute bytecodes via c++ interpreter");
    execute(bytecode, 0, nullptr);
}