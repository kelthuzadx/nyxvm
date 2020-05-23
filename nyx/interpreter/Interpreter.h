#ifndef NYX_INTERPRETER_H
#define NYX_INTERPRETER_H

#include "../bytecode/Bytecode.h"
#include "../bytecode/Opcode.h"
#include "../runtime/Global.h"
#include "../runtime/NObject.h"
#include "Frame.h"

//===----------------------------------------------------------------------===//
// Simple C++ bytecode interpreter
//===----------------------------------------------------------------------===//
class Interpreter {
  private:
    std::vector<Frame*> stack;
    Frame* frame{};

  private:
    template <int Operation> void arithmetic(NObject* o1, NObject* o2);

    template <int Operation> void compare(NObject* o1, NObject* o2);

    template <int Operation> void bitop(NObject* o1, NObject* o2);

    void neg(NObject* object);

    bool deepCompare(int cond, NObject* o1, NObject* o2);

    void createFrame(Bytecode* bytecode, int argc, NObject** argv);

    void destroyFrame(Bytecode* bytecode, bool hasReturnValue);

    void call(Bytecode* bytecodes, int bci);

    void loadFreeVar(FreeVar* freeVar);

    void storeFreeVar(FreeVar* freeVar, NObject* object);

    void execute(Bytecode* bytecode, int argc, NObject** argv);

  public:
    explicit Interpreter();

    ~Interpreter();

    void execute(Bytecode* bytecode);
};

template <int Operation>
void Interpreter::arithmetic(NObject* o1, NObject* o2) {
    switch (Operation) {
    case Opcode::ADD:
        if (typeid(*o1) == typeid(NInt)) {
            NInt* t1 = dynamic_cast<NInt*>(o1);
            if (typeid(*o2) == typeid(NInt)) {
                NInt* t2 = dynamic_cast<NInt*>(o2);
                NInt* res = new NInt(t1->value + t2->value);
                frame->push(res);
            } else if (typeid(*o2) == typeid(NDouble)) {
                auto* t2 = dynamic_cast<NDouble*>(o2);
                auto* res = new NDouble(t1->value + t2->value);
                frame->push(res);
            } else if (typeid(*o2) == typeid(NString)) {
                auto* t2 = dynamic_cast<NString*>(o2);
                auto* res = new NString(std::to_string(t1->value) + t2->value);
                frame->push(res);
            } else {
                panic("should not reach here");
            }
        } else if (typeid(*o1) == typeid(NDouble)) {
            auto* t1 = dynamic_cast<NDouble*>(o1);
            if (typeid(*o2) == typeid(NInt)) {
                NInt* t2 = dynamic_cast<NInt*>(o2);
                auto* res = new NDouble(t1->value + t2->value);
                frame->push(res);
            } else if (typeid(*o2) == typeid(NDouble)) {
                auto* t2 = dynamic_cast<NDouble*>(o2);
                auto* res = new NDouble(t1->value + t2->value);
                frame->push(res);
            } else if (typeid(*o2) == typeid(NString)) {
                auto* t2 = dynamic_cast<NString*>(o2);
                auto* res = new NString(std::to_string(t1->value) + t2->value);
                frame->push(res);
            } else {
                panic("should not reach here");
            }
        } else if (typeid(*o1) == typeid(NString)) {
            auto* t1 = dynamic_cast<NString*>(o1);
            if (typeid(*o2) == typeid(NInt)) {
                NInt* t2 = dynamic_cast<NInt*>(o2);
                auto* res = new NString(t1->value + std::to_string(t2->value));
                frame->push(res);
            } else if (typeid(*o2) == typeid(NDouble)) {
                auto* t2 = dynamic_cast<NDouble*>(o2);
                auto* res = new NString(t1->value + std::to_string(t2->value));
                frame->push(res);
            } else if (typeid(*o2) == typeid(NString)) {
                auto* t2 = dynamic_cast<NString*>(o2);
                auto* res = new NString(t1->value + t2->value);
                frame->push(res);
            } else if (typeid(*o2) == typeid(NChar)) {
                auto* t2 = dynamic_cast<NChar*>(o2);
                std::string str(t1->value);
                str += t2->value;
                auto* res = new NString(str);
                frame->push(res);
            } else {
                panic("should not reach here");
            }
        } else if (typeid(*o1) == typeid(NChar)) {
            auto* t1 = dynamic_cast<NChar*>(o1);
            if (typeid(*o2) == typeid(NString)) {
                auto* t2 = dynamic_cast<NString*>(o2);
                std::string str;
                str += t1->value;
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
        if (typeid(*o1) == typeid(NInt)) {
            NInt* t1 = dynamic_cast<NInt*>(o1);
            if (typeid(*o2) == typeid(NInt)) {
                NInt* t2 = dynamic_cast<NInt*>(o2);
                NInt* res = new NInt(t1->value - t2->value);
                frame->push(res);
            } else if (typeid(*o2) == typeid(NDouble)) {
                auto* t2 = dynamic_cast<NDouble*>(o2);
                auto* res = new NDouble(t1->value - t2->value);
                frame->push(res);
            } else {
                panic("should not reach here");
            }
        } else if (typeid(*o1) == typeid(NDouble)) {
            auto* t1 = dynamic_cast<NDouble*>(o1);
            if (typeid(*o2) == typeid(NInt)) {
                NInt* t2 = dynamic_cast<NInt*>(o2);
                auto* res = new NDouble(t1->value - t2->value);
                frame->push(res);
            } else if (typeid(*o2) == typeid(NDouble)) {
                auto* t2 = dynamic_cast<NDouble*>(o2);
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
        if (typeid(*o1) == typeid(NInt)) {
            NInt* t1 = dynamic_cast<NInt*>(o1);
            if (typeid(*o2) == typeid(NInt)) {
                NInt* t2 = dynamic_cast<NInt*>(o2);
                NInt* res = new NInt(t1->value * t2->value);
                frame->push(res);
            } else if (typeid(*o2) == typeid(NDouble)) {
                auto* t2 = dynamic_cast<NDouble*>(o2);
                auto* res = new NDouble(t1->value * t2->value);
                frame->push(res);
            } else if (typeid(*o2) == typeid(NString)) {
                auto* t2 = dynamic_cast<NString*>(o2);
                std::string str;
                for (int i = 0; i < t1->value; i++) {
                    str += t2->value;
                }
                auto* res = new NString(str);
                frame->push(res);
            } else {
                panic("should not reach here");
            }
        } else if (typeid(*o1) == typeid(NDouble)) {
            auto* t1 = dynamic_cast<NDouble*>(o1);
            if (typeid(*o2) == typeid(NInt)) {
                NInt* t2 = dynamic_cast<NInt*>(o2);
                auto* res = new NDouble(t1->value * t2->value);
                frame->push(res);
            } else if (typeid(*o2) == typeid(NDouble)) {
                auto* t2 = dynamic_cast<NDouble*>(o2);
                auto* res = new NDouble(t1->value * t2->value);
                frame->push(res);
            } else {
                panic("should not reach here");
            }
        } else if (typeid(*o1) == typeid(NString)) {
            auto* t1 = dynamic_cast<NString*>(o1);
            if (typeid(*o2) == typeid(NInt)) {
                auto* t2 = dynamic_cast<NInt*>(o2);
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
        if (typeid(*o1) == typeid(NInt)) {
            NInt* t1 = dynamic_cast<NInt*>(o1);
            if (typeid(*o2) == typeid(NInt)) {
                NInt* t2 = dynamic_cast<NInt*>(o2);
                NInt* res = new NInt(t1->value / t2->value);
                frame->push(res);
            } else if (typeid(*o2) == typeid(NDouble)) {
                auto* t2 = dynamic_cast<NDouble*>(o2);
                auto* res = new NDouble(t1->value / t2->value);
                frame->push(res);
            } else {
                panic("should not reach here");
            }
        } else if (typeid(*o1) == typeid(NDouble)) {
            auto* t1 = dynamic_cast<NDouble*>(o1);
            if (typeid(*o2) == typeid(NInt)) {
                NInt* t2 = dynamic_cast<NInt*>(o2);
                auto* res = new NDouble(t1->value / t2->value);
                frame->push(res);
            } else if (typeid(*o2) == typeid(NDouble)) {
                auto* t2 = dynamic_cast<NDouble*>(o2);
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
        NInt* t1 = dynamic_cast<NInt*>(o1);
        NInt* t2 = dynamic_cast<NInt*>(o2);
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

template <int Operation> void Interpreter::compare(NObject* o1, NObject* o2) {
    if (deepCompare(Operation, o1, o2)) {
        frame->push(new NInt(1));
    } else {
        frame->push(new NInt(0));
    }
}

template <int Operation> void Interpreter::bitop(NObject* o1, NObject* o2) {
    if (o2 == nullptr) {
        // NOT
        auto* t1 = dynamic_cast<NInt*>(o1);
        auto* res = new NInt(~t1->value);
        frame->push(res);
        return;
    }
    // AND OR
    if (typeid(*o1) != typeid(NInt) || typeid(*o2) != typeid(NInt)) {
        panic("bit operation requires integer as its operand");
    }
    auto* t1 = dynamic_cast<NInt*>(o1);
    auto* t2 = dynamic_cast<NInt*>(o2);
    if (Operation == Opcode::AND) {
        auto* res = new NInt((nyx::int32)(t1->value & t2->value));
        frame->push(res);
    } else if (Operation == Opcode::OR) {
        auto* res = new NInt((nyx::int32)(t1->value | t2->value));
        frame->push(res);
    } else {
        panic("should not reach here");
    }
}

#endif // NYX_INTERPRETER_H
