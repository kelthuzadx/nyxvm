#include "Interpreter.h"
#include "../bytecode/Bytecode.h"
#include "../bytecode/Opcode.h"
#include "../runtime/NyxVM.h"

Interpreter::Interpreter() { this->frame = nullptr; }

Interpreter::~Interpreter() {
    frame = nullptr;
    for (auto& val : stack) {
        delete val;
    }
}

void Interpreter::neg(Object* object) {
    if (typeid(*object) == typeid(NInt)) {
        nyx::int32 val = -dynamic_cast<NInt*>(object)->value;
        frame->push(new NInt(val));
    } else if (typeid(*object) == typeid(NDouble)) {
        double val = -dynamic_cast<NDouble*>(object)->value;
        frame->push(new NDouble(val));
    } else {
        panic("should not reach here");
    }
}

void Interpreter::createFrame(Bytecode* bytecode, int argc, Object** argv) {
    // Create execution frame
    this->frame = new Frame(bytecode->localMap.size());
    for (int i = argc - 1, k = 0; i >= 0; i--, k++) {
        auto* obj = argv[i];
        frame->store(k, obj);
    }

    if (!bytecode->freeVars.empty()) {
        for (auto* freeVar : bytecode->freeVars) {
            if (freeVar->isEnclosing) {
                freeVar->value.active = &frame->local()[freeVar->varIndex];
            }
        }
    }

    stack.push_back(frame);
}

void Interpreter::destroyFrame(Bytecode* bytecode, bool hasReturnValue) {
    // Destroy current frame, current frame may points to next frame
    auto* temp = stack.back();
    Object* returnVale = nullptr;
    if (hasReturnValue) {
        returnVale = temp->pop();
    }
    stack.pop_back();
    if (!stack.empty()) {
        this->frame = stack.back();
        if (returnVale != nullptr) {
            this->frame->push(returnVale);
        }
    } else {
        this->frame = nullptr;
    }

    if (!bytecode->freeVars.empty()) {
        // If this is the enclosing frame, we must "move" free variable's value
        // to its endpoint. After that we should set local slot to null in order
        // to avoid deallocating free variable's value
        // Note that we must separate "move" and set actions, since multi
        // endpoints may refer to the same free variable's value, if we move
        // value and immediately set to null, other endpoints will refer to null
        // value.
        for (auto* freeVar : bytecode->freeVars) {
            if (freeVar->isEnclosing) {
                freeVar->endpoint->value.inactive = (*(freeVar->value.active));
            }
        }

        for (auto* freeVar : bytecode->freeVars) {
            if (freeVar->isEnclosing) {
                temp->local()[freeVar->varIndex] = nullptr;
            }
        }
    }
    delete temp;
}

void Interpreter::call(Bytecode* bytecode, int bci) {
    int funcArgc = bytecode->code[bci + 1];

    auto** funcArgv = new Object*[funcArgc];
    for (int k = funcArgc - 1; k >= 0; k--) {
        funcArgv[k] = frame->pop();
    }

    Object* callee = frame->pop();
    if (typeid(*callee) == typeid(NString)) {
        auto funcName = dynamic_cast<NString*>(callee)->value;
        const char* funcPtr = NyxVM::findBuiltin(funcName);
        if (funcPtr != nullptr) {
            Object* result =
                ((Object * (*)(int, Object**)) funcPtr)(funcArgc, funcArgv);
            frame->push(result);
        } else {
            if (auto iter = bytecode->functions.find(funcName);
                iter != bytecode->functions.cend()) {
                Bytecode* userFunc = iter->second;
                this->execute(userFunc, funcArgc, funcArgv);
            } else {
                int closureIndex = bytecode->localMap[funcName];
                if (typeid(*frame->local()[closureIndex]) == typeid(NClosure)) {
                    auto* closureObject =
                        dynamic_cast<NClosure*>(frame->local()[closureIndex]);
                    this->execute(closureObject->code, funcArgc, funcArgv);
                }
            }
        }
    }
    // TODO release funcArgv
}

void Interpreter::execute(Bytecode* bytecode, int argc, Object** argv) {
    createFrame(bytecode, argc, argv);
    auto* code = bytecode->code;
    // Execute bytecode
    {
        for (int bci = 0; bci < bytecode->codeSize; bci++) {
            switch (code[bci]) {
            case Opcode::CALL: {
                call(bytecode, bci);
                bci += 1;
                break;
            }
            case Opcode::CONST_I: {
                nyx::int32 value = *(nyx::int32*)(code + bci + 1);
                auto* object = new NInt(value);
                frame->push(object);
                bci += 4;
                break;
            }
            case Opcode::CONST_D: {
                double value = *(double*)(code + bci + 1);
                auto* object = new NDouble(value);
                frame->push(object);
                bci += 8;
                break;
            }
            case Opcode::CONST_NULL: {
                frame->push(nullptr);
                break;
            }
            case Opcode::CONST_STR: {
                int index = code[bci + 1];
                auto& str = bytecode->strings[index];
                Object* object = new NString(str);
                frame->push(object);
                bci++;
                break;
            }
            case Opcode::ADD: {
                Object* object2 = frame->pop();
                Object* object1 = frame->pop();
                arithmetic<Opcode::ADD>(object1, object2);
                break;
            }
            case Opcode::SUB: {
                Object* object2 = frame->pop();
                Object* object1 = frame->pop();
                arithmetic<Opcode::SUB>(object1, object2);
                break;
            }
            case Opcode::MUL: {
                Object* object2 = frame->pop();
                Object* object1 = frame->pop();
                arithmetic<Opcode::MUL>(object1, object2);
                break;
            }
            case Opcode::DIV: {
                Object* object2 = frame->pop();
                Object* object1 = frame->pop();
                arithmetic<Opcode::DIV>(object1, object2);
                break;
            }
            case Opcode::REM: {
                Object* object2 = frame->pop();
                Object* object1 = frame->pop();
                arithmetic<Opcode::REM>(object1, object2);
                break;
            }
            case Opcode::TEST_EQ: {
                Object* object2 = frame->pop();
                Object* object1 = frame->pop();
                compare<Opcode::TEST_EQ>(object1, object2);
                break;
            }
            case Opcode::TEST_NE: {
                Object* object2 = frame->pop();
                Object* object1 = frame->pop();
                compare<Opcode::TEST_NE>(object1, object2);
                break;
            }
            case Opcode::TEST_GE: {
                Object* object2 = frame->pop();
                Object* object1 = frame->pop();
                compare<Opcode::TEST_GE>(object1, object2);
                break;
            }
            case Opcode::TEST_GT: {
                Object* object2 = frame->pop();
                Object* object1 = frame->pop();
                compare<Opcode::TEST_GT>(object1, object2);
                break;
            }
            case Opcode::TEST_LE: {
                Object* object2 = frame->pop();
                Object* object1 = frame->pop();
                compare<Opcode::TEST_LE>(object1, object2);
                break;
            }
            case Opcode::TEST_LT: {
                Object* object2 = frame->pop();
                Object* object1 = frame->pop();
                compare<Opcode::TEST_LT>(object1, object2);
                break;
            }
            case Opcode::JMP: {
                int target = code[bci + 1];
                bci = target - 1;
                break;
            }
            case Opcode::JMP_NE: {
                auto* cond = dynamic_cast<NInt*>(frame->pop());
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
            case Opcode::JMP_EQ: {
                auto* cond = dynamic_cast<NInt*>(frame->pop());
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
            case Opcode::AND: {
                Object* object2 = frame->pop();
                Object* object1 = frame->pop();
                bitop<Opcode::AND>(object1, object2);
                break;
            }
            case Opcode::OR: {
                Object* object2 = frame->pop();
                Object* object1 = frame->pop();
                bitop<Opcode::OR>(object1, object2);
                break;
            }
            case Opcode::NOT: {
                Object* object1 = frame->pop();
                bitop<Opcode::NOT>(object1, nullptr);
                break;
            }
            case Opcode::NEG: {
                Object* object = frame->pop();
                neg(object);
                break;
            }
            case Opcode::LOAD: {
                int index = code[bci + 1];
                frame->load(index);
                bci++;
                break;
            }
            case Opcode::STORE: {
                Object* value = frame->pop();
                int index = code[bci + 1];
                frame->store(index, value);
                bci++;
                break;
            }
            case Opcode::LOAD_INDEX: {
                auto* index = dynamic_cast<NInt*>(frame->pop());
                auto* array = dynamic_cast<NArray*>(frame->pop());
                auto* elem = array->array[index->value];
                frame->push(elem);
                break;
            }
            case Opcode::STORE_INDEX: {
                auto* index = dynamic_cast<NInt*>(frame->pop());
                auto* object = frame->pop();
                auto* array = dynamic_cast<NArray*>(frame->pop());
                array->array[index->value] = object;
                break;
            }
            case Opcode::NEW_ARR: {
                int length = code[bci + 1];
                frame->push(new NArray(length));
                bci++;
                break;
            }
            case Opcode::DUP: {
                frame->dup();
                break;
            }
            case Opcode::RETURN: {
                destroyFrame(bytecode, false);
                return;
            }
            case Opcode::RETURN_VAL: {
                destroyFrame(bytecode, true);
                return;
            }
            case Opcode::ARR_LEN: {
                auto* array = frame->pop();
                if (typeid(*array) != typeid(NArray)) {
                    panic("array length is only operated on Array type");
                }
                int length = dynamic_cast<NArray*>(array)->length;
                frame->push(new NInt(length));
                break;
            }
            case Opcode::CONST_CLOSURE: {
                int closureIndex = code[bci + 1];
                frame->push(new NClosure(bytecode->closures[closureIndex]));
                bci++;
                break;
            }
            case Opcode::LOAD_FREE: {
                int freeIndex = code[bci + 1];
                FreeVar* freeVar = bytecode->freeVars[freeIndex];
                if (freeVar->value.inactive != nullptr) {
                    // Enclosing context is inactive, current context owns the
                    // free variable
                    frame->push(freeVar->value.inactive);
                } else {
                    // Enclosing context is still active, current context just
                    // "borrows" free variable
                    frame->push(*(freeVar->endpoint->value.active));
                }
                bci++;
                break;
            }
            case Opcode::STORE_FREE: {
                auto* object = frame->pop();
                int freeIndex = code[bci + 1];
                FreeVar* freeVar = bytecode->freeVars[freeIndex];
                if (freeVar->value.inactive != nullptr) {
                    // Enclosing context is inactive, current context owns the
                    // free variable
                    freeVar->value.inactive = object;
                } else {
                    // Enclosing context is still active, current context just
                    // "borrows" free variable
                    *(freeVar->endpoint->value.active) = object;
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

void Interpreter::execute(Bytecode* bytecode) {
    PhaseTime timer("execute bytecodes via c++ interpreter");
    execute(bytecode, 0, nullptr);
}