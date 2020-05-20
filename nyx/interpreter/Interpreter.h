#ifndef NYX_INTERPRETER_H
#define NYX_INTERPRETER_H

#include "../runtime/Global.h"
#include "../bytecode/Bytecode.h"
#include "../runtime/Object.h"
#include "../bytecode/Opcode.h"
#include "Frame.h"

class Interpreter {
private:
    std::vector<Frame *> stack;
    Frame *frame{};

private:
    template<int Operation>
    void arithmetic(Object *o1, Object *o2);

    template<int Operation>
    void compare(Object *o1, Object *o2);

    template<int Operation>
    void bitop(Object *o1, Object *o2);

    void neg(Object *object);

    void createFrame(Bytecode *bytecode, int argc, Object **argv);

    void destroyFrame(Bytecode *bytecode,bool hasReturnValue);

    void call(Bytecode* bytecodes, int bci);

    void execute(Bytecode *bytecode, int argc, Object **argv);

public:
    explicit Interpreter();

    ~Interpreter();

    void execute(Bytecode *bytecode);
};

template<int Operation>
void Interpreter::arithmetic(Object *o1, Object *o2) {
    switch (Operation) {
        case ADD:
            if (typeid(*o1) == typeid(NInt)) {
                NInt *t1 = dynamic_cast<NInt *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    NInt *res = new NInt(t1->value + t2->value);
                    frame->push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value + t2->value);
                    frame->push(res);
                } else if (typeid(*o2) == typeid(NString)) {
                    auto *t2 = dynamic_cast<NString *>(o2);
                    auto *res = new NString(std::to_string(t1->value) + t2->value);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (typeid(*o1) == typeid(NDouble)) {
                auto *t1 = dynamic_cast<NDouble *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    auto *res = new NDouble(t1->value + t2->value);
                    frame->push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value + t2->value);
                    frame->push(res);
                } else if (typeid(*o2) == typeid(NString)) {
                    auto *t2 = dynamic_cast<NString *>(o2);
                    auto *res = new NString(std::to_string(t1->value) + t2->value);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (typeid(*o1) == typeid(NString)) {
                auto *t1 = dynamic_cast<NString *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    auto *res = new NString(t1->value + std::to_string(t2->value));
                    frame->push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NString(t1->value + std::to_string(t2->value));
                    frame->push(res);
                } else if (typeid(*o2) == typeid(NString)) {
                    auto *t2 = dynamic_cast<NString *>(o2);
                    auto *res = new NString(t1->value + t2->value);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else {
                panic("should not reach here");
            }
            break;
        case SUB:
            if (typeid(*o1) == typeid(NInt)) {
                NInt *t1 = dynamic_cast<NInt *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    NInt *res = new NInt(t1->value - t2->value);
                    frame->push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value - t2->value);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (typeid(*o1) == typeid(NDouble)) {
                auto *t1 = dynamic_cast<NDouble *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    auto *res = new NDouble(t1->value - t2->value);
                    frame->push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value - t2->value);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else {
                panic("should not reach here");
            }
            break;
        case MUL:
            if (typeid(*o1) == typeid(NInt)) {
                NInt *t1 = dynamic_cast<NInt *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    NInt *res = new NInt(t1->value * t2->value);
                    frame->push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value * t2->value);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (typeid(*o1) == typeid(NDouble)) {
                auto *t1 = dynamic_cast<NDouble *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    auto *res = new NDouble(t1->value * t2->value);
                    frame->push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value * t2->value);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else {
                panic("should not reach here");
            }
            break;
        case DIV:
            if (typeid(*o1) == typeid(NInt)) {
                NInt *t1 = dynamic_cast<NInt *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    NInt *res = new NInt(t1->value / t2->value);
                    frame->push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value / t2->value);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (typeid(*o1) == typeid(NDouble)) {
                auto *t1 = dynamic_cast<NDouble *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    auto *res = new NDouble(t1->value / t2->value);
                    frame->push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value / t2->value);
                    frame->push(res);
                } else {
                    panic("should not reach here");
                }
            } else {
                panic("should not reach here");
            }
            break;
        case REM: {
            if (typeid(*o1) != typeid(NInt) || typeid(*o2) != typeid(NInt)) {
                panic("operator % needs both integer operand");
            }
            NInt *t1 = dynamic_cast<NInt *>(o1);
            NInt *t2 = dynamic_cast<NInt *>(o2);
            NInt *res = new NInt(t1->value % t2->value);
            frame->push(res);
            break;
        }
        default:
            panic("should not reach here");
    }
}

template<typename T1, typename T2>
bool genericCompare(int operation, T1 *t1, T2 *t2) {
    bool cond;
    switch (operation) {
        case TEST_EQ:
            cond = t1->value == t2->value;
            break;
        case TEST_NE:
            cond = t1->value != t2->value;
            break;
        case TEST_GE:
            cond = t1->value >= t2->value;
            break;
        case TEST_GT:
            cond = t1->value > t2->value;
            break;
        case TEST_LE:
            cond = t1->value <= t2->value;
            break;
        case TEST_LT:
            cond = t1->value < t2->value;
            break;
        default:
            panic("should not reach here");
    }
    return cond;
}

template<int Operation>
void Interpreter::compare(Object *o1, Object *o2) {
    if (typeid(*o1) == typeid(NInt) && typeid(*o2) == typeid(NInt)) {
        auto *t1 = dynamic_cast<NInt *>(o1);
        auto *t2 = dynamic_cast<NInt *>(o2);
        NInt *res = new NInt(genericCompare(Operation, t1, t2));
        frame->push(res);
    } else if (typeid(*o1) == typeid(NDouble) && typeid(*o2) == typeid(NDouble)) {
        auto *t1 = dynamic_cast<NDouble *>(o1);
        auto *t2 = dynamic_cast<NDouble *>(o2);
        NInt *res = new NInt(genericCompare(Operation, t1, t2));
        frame->push(res);
    } else if (typeid(*o1) == typeid(NString) && typeid(*o2) == typeid(NString)) {
        auto *t1 = dynamic_cast<NString *>(o1);
        auto *t2 = dynamic_cast<NString *>(o2);
        NInt *res = new NInt(genericCompare(Operation, t1, t2));
        frame->push(res);
    } else {
        panic("should not reach here");
    }
}

template<int Operation>
void Interpreter::bitop(Object *o1, Object *o2) {
    if (o2 == nullptr) {
        // NOT
        auto *t1 = dynamic_cast<NInt *>(o1);
        auto *res = new NInt(~t1->value);
        frame->push(res);
        return;
    }
    // AND OR
    if (typeid(*o1) != typeid(NInt) || typeid(*o2) != typeid(NInt)) {
        panic("bit operation requires integer as its operand");
    }
    auto *t1 = dynamic_cast<NInt *>(o1);
    auto *t2 = dynamic_cast<NInt *>(o2);
    if (Operation == AND) {
        auto *res = new NInt((nyx::int32) (t1->value & t2->value));
        frame->push(res);
    } else if (Operation == OR) {
        auto *res = new NInt((nyx::int32) (t1->value | t2->value));
        frame->push(res);
    } else {
        panic("should not reach here");
    }
}

#endif //NYX_INTERPRETER_H
