#ifndef NYX_INTERPRETER_H
#define NYX_INTERPRETER_H

#include "../bytecode/Bytecode.h"
#include "../bytecode/Opcode.h"
#include "../gc/GenHeap.h"
#include "../object/NValue.h"
#include "../runtime/Global.h"
#include "Frame.h"

//===----------------------------------------------------------------------===//
// Simple C++ bytecode interpreter
//===----------------------------------------------------------------------===//
class Interpreter {
  private:
    std::vector<Frame*> stack;
    Frame* frame{};

  private:
    template <int Operation> void arithmetic(NValue* o1, NValue* o2);

    template <int Operation> void compare(NValue* o1, NValue* o2);

    template <int Operation> void bitop(NValue* o1, NValue* o2);

    void neg(NValue* object);

    bool deepCompare(int cond, NValue* o1, NValue* o2);

    void createFrame(Bytecode* bytecode, int argc, NValue** argv);

    void destroyFrame(Bytecode* bytecode, bool hasReturnValue);

    void call(Bytecode* bytecodes, int bci);

    void loadFreeVar(FreeVar* freeVar);

    void storeFreeVar(FreeVar* freeVar, NValue* object);

    void execute(Bytecode* bytecode, int argc, NValue** argv);

  public:
    explicit Interpreter();

    ~Interpreter();

    void execute(Bytecode* bytecode);
};


template <int Operation> void Interpreter::arithmetic(NValue* o1, NValue* o2) {
    switch (Operation) {
        case Opcode::ADD:
            if (is<NInt>(o1)) {
                NInt* t1 = as<NInt>(o1);
                if (is<NInt>(o2)) {
                    NInt* t2 = as<NInt>(o2);
                    NInt* res = GenHeap::instance().allocateNInt(t1->getValue()+t2->getValue());
                    frame->push(res);
                } else if (is<NDouble>(o2)) {
                    auto* t2 = as<NDouble>(o2);
                    auto* res =  GenHeap::instance().allocateNDouble(t1->getValue()+t2->getValue());
                    frame->push(res);
                } else if (is<NString>(o2)) {
                    auto* t2 = as<NString>(o2);
                    std::string result = std::to_string(t1->) + t2->value;
                    auto* res = new NString(s);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (is<NDouble>(o1)) {
                auto* t1 = as<NDouble>(o1);
                if (is<NInt>(o2)) {
                    NInt* t2 = as<NInt>(o2);
                    auto* res = new NDouble(t1->value + t2->value);
                    frame->push(res);
                } else if (is<NDouble>(o2)) {
                    auto* t2 = as<NDouble>(o2);
                    auto* res = new NDouble(t1->value + t2->value);
                    frame->push(res);
                } else if (is<NString>(o2)) {
                    auto* t2 = as<NString>(o2);
                    auto* res = new NString(std::to_string(t1->value) + t2->value);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (is<NString>(o1)) {
                auto* t1 = as<NString>(o1);
                if (is<NInt>(o2)) {
                    NInt* t2 = as<NInt>(o2);
                    auto* res = new NString(t1->value + std::to_string(t2->value));
                    frame->push(res);
                } else if (is<NDouble>(o2)) {
                    auto* t2 = as<NDouble>(o2);
                    auto* res = new NString(t1->value + std::to_string(t2->value));
                    frame->push(res);
                } else if (is<NString>(o2)) {
                    auto* t2 = as<NString>(o2);
                    auto* res = new NString(t1->value + t2->value);
                    frame->push(res);
                } else if (is<NChar>(o2)) {
                    auto* t2 = as<NChar>(o2);
                    std::string str(t1->value);
                    str += t2->value;
                    auto* res = new NString(str);
                    frame->push(res);
                } else if (is<NArray>(o2)) {
                    auto* t2 = as<NArray>(o2);
                    std::string str(t1->value);
                    str += t2->toString();
                    auto* res = new NString(str);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (is<NChar>(o1)) {
                auto* t1 = as<NChar>(o1);
                if (is<NString>(o2)) {
                    auto* t2 = as<NString>(o2);
                    std::string str;
                    str += t1->value;
                    str += t2->value;
                    auto* res = new NString(str);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (is<NArray>(o1)) {
                auto* t1 = as<NArray>(o1);
                if (is<NString>(o2)) {
                    auto* t2 = as<NString>(o2);
                    std::string str;
                    str += t1->toString();
                    str += t2->value;
                    auto* res = new NString(str);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else {
                panic("should not reach here");
            }
            break;
        case Opcode::SUB:
            if (is<NInt>(o1)) {
                NInt* t1 = as<NInt>(o1);
                if (is<NInt>(o2)) {
                    NInt* t2 = as<NInt>(o2);
                    NInt* res = new NInt(t1->value - t2->value);
                    frame->push(res);
                } else if (is<NDouble>(o2)) {
                    auto* t2 = as<NDouble>(o2);
                    auto* res = new NDouble(t1->value - t2->value);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (is<NDouble>(o1)) {
                auto* t1 = as<NDouble>(o1);
                if (is<NInt>(o2)) {
                    NInt* t2 = as<NInt>(o2);
                    auto* res = new NDouble(t1->value - t2->value);
                    frame->push(res);
                } else if (is<NDouble>(o2)) {
                    auto* t2 = as<NDouble>(o2);
                    auto* res = new NDouble(t1->value - t2->value);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else {
                panic("should not reach here");
            }
            break;
        case Opcode::MUL:
            if (is<NInt>(o1)) {
                NInt* t1 = as<NInt>(o1);
                if (is<NInt>(o2)) {
                    NInt* t2 = as<NInt>(o2);
                    NInt* res = new NInt(t1->value * t2->value);
                    frame->push(res);
                } else if (is<NDouble>(o2)) {
                    auto* t2 = as<NDouble>(o2);
                    auto* res = new NDouble(t1->value * t2->value);
                    frame->push(res);
                } else if (is<NString>(o2)) {
                    auto* t2 = as<NString>(o2);
                    std::string str;
                    for (int i = 0; i < t1->value; i++) {
                        str += t2->value;
                    }
                    auto* res = new NString(str);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (is<NDouble>(o1)) {
                auto* t1 = as<NDouble>(o1);
                if (is<NInt>(o2)) {
                    NInt* t2 = as<NInt>(o2);
                    auto* res = new NDouble(t1->value * t2->value);
                    frame->push(res);
                } else if (is<NDouble>(o2)) {
                    auto* t2 = as<NDouble>(o2);
                    auto* res = new NDouble(t1->value * t2->value);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (is<NString>(o1)) {
                auto* t1 = as<NString>(o1);
                if (is<NInt>(o2)) {
                    auto* t2 = as<NInt>(o2);
                    std::string str;
                    for (int i = 0; i < t2->value; i++) {
                        str += t1->value;
                    }
                    auto* res = new NString(str);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else {
                panic("should not reach here");
            }
            break;
        case Opcode::DIV:
            if (is<NInt>(o1)) {
                NInt* t1 = as<NInt>(o1);
                if (is<NInt>(o2)) {
                    NInt* t2 = as<NInt>(o2);
                    NInt* res = new NInt(t1->value / t2->value);
                    frame->push(res);
                } else if (is<NDouble>(o2)) {
                    auto* t2 = as<NDouble>(o2);
                    auto* res = new NDouble(t1->value / t2->value);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (is<NDouble>(o1)) {
                auto* t1 = as<NDouble>(o1);
                if (is<NInt>(o2)) {
                    NInt* t2 = as<NInt>(o2);
                    auto* res = new NDouble(t1->value / t2->value);
                    frame->push(res);
                } else if (is<NDouble>(o2)) {
                    auto* t2 = as<NDouble>(o2);
                    auto* res = new NDouble(t1->value / t2->value);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else {
                panic("should not reach here");
            }
            break;
        case Opcode::REM: {
            if (typeid(*o1) != typeid(NInt) || typeid(*o2) != typeid(NInt)) {
                panic("operator % needs both integer operand");
            }
            NInt* t1 = as<NInt>(o1);
            NInt* t2 = as<NInt>(o2);
            NInt* res = new NInt(t1->value % t2->value);
            frame->push(res);
            break;
        }
        default:
            panic("should not reach here");
    }
}

template <typename T1, typename T2>
bool genericCompare(int operation, T1* t1, T2* t2) {
    bool cond;
    switch (operation) {
        case Opcode::TEST_EQ:
            cond = t1->value == t2->value;
            break;
        case Opcode::TEST_NE:
            cond = t1->value != t2->value;
            break;
        case Opcode::TEST_GE:
            cond = t1->value >= t2->value;
            break;
        case Opcode::TEST_GT:
            cond = t1->value > t2->value;
            break;
        case Opcode::TEST_LE:
            cond = t1->value <= t2->value;
            break;
        case Opcode::TEST_LT:
            cond = t1->value < t2->value;
            break;
        default:
            panic("should not reach here");
    }
    return cond;
}

template <int Operation> void Interpreter::compare(NValue* o1, NValue* o2) {
    frame->push(GenHeap::instance().allocateNInt(
        deepCompare(Operation, o1, o2) ? 1 : 0));
}

template <int Operation> void Interpreter::bitop(NValue* o1, NValue* o2) {
    if (o2 == nullptr) {
        // NOT
        auto* t1 = as<NInt>(o1);
        auto* res = GenHeap::instance().allocateNInt(~t1->getValue());
        frame->push(res);
        return;
    }
    // AND OR
    if (typeid(*o1) != typeid(NInt) || typeid(*o2) != typeid(NInt)) {
        panic("bit operation requires integer as its operand");
    }
    auto* t1 = as<NInt>(o1);
    auto* t2 = as<NInt>(o2);
    if (Operation == Opcode::AND) {
        auto* res = GenHeap::instance().allocateNInt(t1->getValue() & t2->getValue());
        frame->push(res);
    } else if (Operation == Opcode::OR) {
        auto* res = GenHeap::instance().allocateNInt(t1->getValue() | t2->getValue());
        frame->push(res);
    } else {
        panic("should not reach here");
    }
}
#endif // NYX_INTERPRETER_H
