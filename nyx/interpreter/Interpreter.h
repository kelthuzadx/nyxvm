#ifndef NYX_INTERPRETER_H
#define NYX_INTERPRETER_H

#include "../runtime/Global.h"
#include "../bytecode/MetaArea.h"
#include "../runtime/Object.h"
#include "../bytecode/Opcode.h"
#include "Frame.h"

class Interpreter {
private:
    MetaArea *meta{};
    std::vector<Frame *> stack;
    Frame *frame{};

private:
    Object *pop();

    void push(Object *obj);

    void load(int index);

    void store(int index, Object *value);

    template<int Operation>
    void arithmetic(Object *o1, Object *o2);

    template<int Operation>
    void compare(Object *o1, Object *o2);

    template<int Operation>
    void bitop(Object *o1, Object *o2);

    void neg(Object *object);

public:
    explicit Interpreter(MetaArea *meta);

    void execute();
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
                    push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value + t2->value);
                    push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (typeid(*o1) == typeid(NDouble)) {
                auto *t1 = dynamic_cast<NDouble *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    auto *res = new NDouble(t1->value + t2->value);
                    push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value + t2->value);
                    push(res);
                } else {
                    panic("should not reach here");
                }
            } else {
                panic("should not reach here");
            }
        case SUB:
            if (typeid(*o1) == typeid(NInt)) {
                NInt *t1 = dynamic_cast<NInt *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    NInt *res = new NInt(t1->value - t2->value);
                    push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value - t2->value);
                    push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (typeid(*o1) == typeid(NDouble)) {
                auto *t1 = dynamic_cast<NDouble *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    auto *res = new NDouble(t1->value - t2->value);
                    push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value - t2->value);
                    push(res);
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
                    push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value * t2->value);
                    push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (typeid(*o1) == typeid(NDouble)) {
                auto *t1 = dynamic_cast<NDouble *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    auto *res = new NDouble(t1->value * t2->value);
                    push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value * t2->value);
                    push(res);
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
                    push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value / t2->value);
                    push(res);
                } else {
                    panic("should not reach here");
                }
            } else if (typeid(*o1) == typeid(NDouble)) {
                auto *t1 = dynamic_cast<NDouble *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    auto *res = new NDouble(t1->value / t2->value);
                    push(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value / t2->value);
                    push(res);
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
            push(res);
            break;
        }
        default:
            panic("should not reach here");
    }
}


template<int Operation>
void Interpreter::compare(Object *o1, Object *o2) {
    // TEST
    if (o2 == nullptr) {
        if (typeid(*o1) == typeid(NInt)) {
            auto *t1 = dynamic_cast<NInt *>(o1);
            auto *res = new NInt(t1->value == 0);
            push(res);
        } else {
            panic("should not reach here");
        }
        return;
    }
    // Other test bytecodes
    if (typeid(*o1) == typeid(NInt) && typeid(*o2) == typeid(NInt)) {
        auto *t1 = dynamic_cast<NInt *>(o1);
        auto *t2 = dynamic_cast<NInt *>(o2);
        bool cond;
        switch (Operation) {
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
        auto *res = new NInt(cond);
        push(res);
    } else if (typeid(*o1) == typeid(NDouble) && typeid(*o2) == typeid(NDouble)) {
        auto *t1 = dynamic_cast<NDouble *>(o1);
        auto *t2 = dynamic_cast<NDouble *>(o2);
        bool cond;
        switch (Operation) {
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
        auto *res = new NInt(cond);
        push(res);
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
        push(res);
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
        push(res);
    } else if (Operation == OR) {
        auto *res = new NInt((nyx::int32) (t1->value | t2->value));
        push(res);
    } else {
        panic("should not reach here");
    }
}

#endif //NYX_INTERPRETER_H
