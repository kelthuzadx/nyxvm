#include "Interpreter.h"
#include "../bytecode/Bytecode.h"
#include "../bytecode/Opcode.h"
#include "../gc/GenHeap.h"
#include "../object/NValue.h"
#include "../runtime/NyxVM.h"

Interpreter::Interpreter() { this->frame = nullptr; }

Interpreter::~Interpreter() {
    frame = nullptr;
    for (auto& val : stack) {
        delete val;
    }
}

void Interpreter::neg(NValue* object) {
    if (typeid(*object) == typeid(NInt)) {
        int32 val = -as<NInt>(object)->getValue();
        frame->push(GenHeap::instance().allocateNInt(val));
    } else if (typeid(*object) == typeid(NDouble)) {
        double val = -as<NDouble>(object)->getValue();
        frame->push(GenHeap::instance().allocateNDouble(val));
    } else {
        panic("should not reach here");
    }
}

bool Interpreter::deepCompare(int cond, NValue* o1, NValue* o2) {
    if (o1 == nullptr || o2 == nullptr) {
        return cond == Opcode::TEST_EQ == (o1 == nullptr && o2 == nullptr);
    }

    if (is<NInt>(o1) && is<NInt>(o2)) {
        auto* t1 = as<NInt>(o1);
        auto* t2 = as<NInt>(o2);
        return genericCompare(cond, t1, t2);
    } else if (is<NDouble>(o1) && is<NDouble>(o2)) {
        auto* t1 = as<NDouble>(o1);
        auto* t2 = as<NDouble>(o2);
        return genericCompare(cond, t1, t2);
    } else if (is<NChar>(o1) && is<NChar>(o2)) {
        auto* t1 = as<NChar>(o1);
        auto* t2 = as<NChar>(o2);
        return genericCompare(cond, t1, t2);
    } else if (is<NString>(o1) && is<NString>(o2)) {
        auto* t1 = as<NString>(o1);
        auto* t2 = as<NString>(o2);
        return genericCompare(cond, t1, t2);
    } else if (is<NArray>(o1) && is<NArray>(o2)) {
        assert(cond == Opcode::TEST_EQ || cond == Opcode::TEST_NE);
        auto* t1 = as<NArray>(o1);
        auto* t2 = as<NArray>(o2);
        if (t1->getLength() != t2->getLength()) {
            return Opcode::TEST_EQ != cond;
        }
        for (int i = 0; i < t1->getLength(); i++) {
            if (!deepCompare(Opcode::TEST_EQ, t1->getElement(i),
                             t2->getElement(i))) {
                return Opcode::TEST_EQ != cond;
            }
        }

        return Opcode::TEST_EQ == cond;
    } else {
        panic("should not reach here");
    }
}

void Interpreter::createFrame(Bytecode* bytecode, int argc, NValue** argv) {
    // Create execution frame
    this->frame = new Frame(bytecode->localVars.size());
    for (int i = 0; i < argc; i++) {
        auto* obj = argv[i];
        frame->store(i, obj);
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
    NValue* returnVale = nullptr;
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
        // to avoid releasing free variable's value
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

void Interpreter::loadFreeVar(FreeVar* freeVar) {
    if (freeVar->value.inactive != nullptr) {
        // Enclosing context is inactive, current context owns the
        // free variable
        frame->push(freeVar->value.inactive);
    } else {
        // Enclosing context is still active, current context just
        // "borrows" free variable
        frame->push(*(freeVar->endpoint->value.active));
    }
}

void Interpreter::storeFreeVar(FreeVar* freeVar, NValue* object) {
    if (freeVar->value.inactive != nullptr) {
        // Enclosing context is inactive, current context owns the
        // free variable
        freeVar->value.inactive = object;
    } else {
        // Enclosing context is still active, current context just
        // "borrows" free variable
        *(freeVar->endpoint->value.active) = object;
    }
}

void Interpreter::call(Bytecode* bytecode, int bci) {
    int funcArgc = bytecode->code[bci + 1];

    auto** funcArgv = new NValue*[funcArgc];
    for (int k = funcArgc - 1; k >= 0; k--) {
        funcArgv[k] = frame->pop();
    }

    NValue* callee = frame->pop();
    if (typeid(*callee) != typeid(NCallable)) {
        panic("callee '%s' is not callable");
    }
    auto* callable = as<NCallable>(callee);
    if (callable->isNative) {
        NValue* result = ((NValue * (*)(int, NValue**))(
            (const char*)callable->code.native))(funcArgc, funcArgv);
        frame->push(result);
        return;
    } else {
        this->execute(callable->code.bytecode, funcArgc, funcArgv);
        return;
    }
    // TODO release funcArgv
}

void Interpreter::execute(Bytecode* bytecode, int argc, NValue** argv) {
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
                int32 value = *(int32*)(code + bci + 1);
                auto* object = GenHeap::instance().allocateNInt(value);
                frame->push(object);
                bci += 4;
                break;
            }
            case Opcode::CONST_D: {
                double value = *(double*)(code + bci + 1);
                auto* object = GenHeap::instance().allocateNDouble(value);
                frame->push(object);
                bci += 8;
                break;
            }
            case Opcode::CONST_C: {
                int8 value = *(int8*)(code + bci + 1);
                auto* object = GenHeap::instance().allocateNChar(value);
                frame->push(object);
                bci += 1;
                break;
            }
            case Opcode::CONST_NULL: {
                frame->push(nullptr);
                break;
            }
            case Opcode::CONST_STR: {
                int index = code[bci + 1];
                auto& str = bytecode->strings[index];
                NValue* object = GenHeap::instance().allocateNString(str);
                frame->push(object);
                bci++;
                break;
            }
            case Opcode::ADD: {
                NValue* object2 = frame->pop();
                NValue* object1 = frame->pop();
                arithmetic<Opcode::ADD>(object1, object2);
                break;
            }
            case Opcode::SUB: {
                NValue* object2 = frame->pop();
                NValue* object1 = frame->pop();
                arithmetic<Opcode::SUB>(object1, object2);
                break;
            }
            case Opcode::MUL: {
                NValue* object2 = frame->pop();
                NValue* object1 = frame->pop();
                arithmetic<Opcode::MUL>(object1, object2);
                break;
            }
            case Opcode::DIV: {
                NValue* object2 = frame->pop();
                NValue* object1 = frame->pop();
                arithmetic<Opcode::DIV>(object1, object2);
                break;
            }
            case Opcode::REM: {
                NValue* object2 = frame->pop();
                NValue* object1 = frame->pop();
                arithmetic<Opcode::REM>(object1, object2);
                break;
            }
            case Opcode::TEST_EQ: {
                NValue* object2 = frame->pop();
                NValue* object1 = frame->pop();
                compare<Opcode::TEST_EQ>(object1, object2);
                break;
            }
            case Opcode::TEST_NE: {
                NValue* object2 = frame->pop();
                NValue* object1 = frame->pop();
                compare<Opcode::TEST_NE>(object1, object2);
                break;
            }
            case Opcode::TEST_GE: {
                NValue* object2 = frame->pop();
                NValue* object1 = frame->pop();
                compare<Opcode::TEST_GE>(object1, object2);
                break;
            }
            case Opcode::TEST_GT: {
                NValue* object2 = frame->pop();
                NValue* object1 = frame->pop();
                compare<Opcode::TEST_GT>(object1, object2);
                break;
            }
            case Opcode::TEST_LE: {
                NValue* object2 = frame->pop();
                NValue* object1 = frame->pop();
                compare<Opcode::TEST_LE>(object1, object2);
                break;
            }
            case Opcode::TEST_LT: {
                NValue* object2 = frame->pop();
                NValue* object1 = frame->pop();
                compare<Opcode::TEST_LT>(object1, object2);
                break;
            }
            case Opcode::JMP: {
                int target = code[bci + 1];
                bci = target - 1;
                break;
            }
            case Opcode::JMP_NE: {
                auto* cond = as<NInt>(frame->pop());
                if (cond->getValue() == 0) {
                    // Goto target
                    int target = code[bci + 1];
                    bci = target - 1;
                } else {
                    // Otherwise, just forward bci
                    assert(cond->getValue() == 1);
                    bci++;
                }
                break;
            }
            case Opcode::JMP_EQ: {
                auto* cond = as<NInt>(frame->pop());
                if (cond->getValue() == 1) {
                    // Goto target
                    int target = code[bci + 1];
                    bci = target - 1;
                } else {
                    // Otherwise, just forward bci
                    assert(cond->getValue() == 0);
                    bci++;
                }
                break;
            }
            case Opcode::AND: {
                NValue* object2 = frame->pop();
                NValue* object1 = frame->pop();
                bitop<Opcode::AND>(object1, object2);
                break;
            }
            case Opcode::OR: {
                NValue* object2 = frame->pop();
                NValue* object1 = frame->pop();
                bitop<Opcode::OR>(object1, object2);
                break;
            }
            case Opcode::NOT: {
                NValue* object1 = frame->pop();
                bitop<Opcode::NOT>(object1, nullptr);
                break;
            }
            case Opcode::NEG: {
                NValue* object = frame->pop();
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
                NValue* value = frame->pop();
                int index = code[bci + 1];
                frame->store(index, value);
                bci++;
                break;
            }
            case Opcode::LOAD_INDEX: {
                auto* index = as<NInt>(frame->pop());
                auto* array = as<NArray>(frame->pop());
                auto* elem = array->getElement(index->getValue());
                frame->push(elem);
                break;
            }
            case Opcode::STORE_INDEX: {
                auto* index = as<NInt>(frame->pop());
                auto* object = frame->pop();
                auto* array = as<NArray>(frame->pop());
                array->setElement(index->getValue(), object);
                break;
            }
            case Opcode::NEW_ARR: {
                int length = code[bci + 1];
                frame->push(GenHeap::instance().allocateNArray(length));
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
                uint32 length = as<NArray>(array)->getLength();
                frame->push(GenHeap::instance().allocateNInt(length));
                break;
            }
            case Opcode::CONST_CALLABLE: {
                int callableIndex = code[bci + 1];
                if (callableIndex >= 0) {
                    Bytecode* temp = bytecode;
                    while (temp != nullptr) {
                        if (temp->callables.find(callableIndex) !=
                            temp->callables.end()) {
                            frame->push(GenHeap::instance().allocateNCallable(
                                false, reinterpret_cast<pointer>(
                                           temp->callables[callableIndex])));
                            break;
                        }
                        temp = temp->parent;
                    }
                } else {
                    frame->push(GenHeap::instance().allocateNCallable(
                        true, const_cast<pointer>(
                                  bytecode->builtin[-callableIndex - 1][1])));
                }
                bci++;
                break;
            }
            case Opcode::LOAD_FREE: {
                int freeIndex = code[bci + 1];
                FreeVar* freeVar = bytecode->freeVars[freeIndex];
                loadFreeVar(freeVar);
                bci++;
                break;
            }
            case Opcode::STORE_FREE: {
                auto* object = frame->pop();
                int freeIndex = code[bci + 1];
                FreeVar* freeVar = bytecode->freeVars[freeIndex];
                storeFreeVar(freeVar, object);
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