#ifndef NYX_INTERPRETER_H
#define NYX_INTERPRETER_H

#include "../runtime/Global.hpp"
#include "../bytecode/MetaArea.h"
#include "../runtime/Object.h"
#include "../bytecode/Opcode.h"

struct Frame {
    std::vector<Object *> slots;
};

class Interpreter {
private:
    MetaArea *meta{};
    std::vector<Frame *> stack;
    Frame *frame;
private:
    const char *findBuiltin(const std::string &name);

    template<int Operation>
    void arithmetic(Object *o1, Object *o2);

public:
    Interpreter(MetaArea *meta);

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
                    frame->slots.push_back(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value + t2->value);
                    frame->slots.push_back(res);
                } else {
                    panic("should not reach here");
                }
            } else if (typeid(*o1) == typeid(NDouble)) {
                auto *t1 = dynamic_cast<NDouble *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    auto *res = new NDouble(t1->value + t2->value);
                    frame->slots.push_back(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value + t2->value);
                    frame->slots.push_back(res);
                } else {

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
                    frame->slots.push_back(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value - t2->value);
                    frame->slots.push_back(res);
                } else {
                    panic("should not reach here");
                }
            } else if (typeid(*o1) == typeid(NDouble)) {
                auto *t1 = dynamic_cast<NDouble *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    auto *res = new NDouble(t1->value - t2->value);
                    frame->slots.push_back(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value - t2->value);
                    frame->slots.push_back(res);
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
                    frame->slots.push_back(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value * t2->value);
                    frame->slots.push_back(res);
                } else {
                    panic("should not reach here");
                }
            } else if (typeid(*o1) == typeid(NDouble)) {
                auto *t1 = dynamic_cast<NDouble *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    auto *res = new NDouble(t1->value * t2->value);
                    frame->slots.push_back(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value * t2->value);
                    frame->slots.push_back(res);
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
                    frame->slots.push_back(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value / t2->value);
                    frame->slots.push_back(res);
                } else {
                    panic("should not reach here");
                }
            } else if (typeid(*o1) == typeid(NDouble)) {
                auto *t1 = dynamic_cast<NDouble *>(o1);
                if (typeid(*o2) == typeid(NInt)) {
                    NInt *t2 = dynamic_cast<NInt *>(o2);
                    auto *res = new NDouble(t1->value / t2->value);
                    frame->slots.push_back(res);
                } else if (typeid(*o2) == typeid(NDouble)) {
                    auto *t2 = dynamic_cast<NDouble *>(o2);
                    auto *res = new NDouble(t1->value / t2->value);
                    frame->slots.push_back(res);
                } else {
                    panic("should not reach here");
                }
            } else {
                panic("should not reach here");
            }
            break;
        case REM: {
            if (typeid(*o1) != typeid(NInt) || typeid(*o2) != typeid(NInt)) {
                panic("should not reach here");
            }
            NInt *t1 = dynamic_cast<NInt *>(o1);
            NInt *t2 = dynamic_cast<NInt *>(o2);
            NInt *res = new NInt(t1->value % t2->value);
            frame->slots.push_back(res);
            break;
        }
        default:
            panic("should not reach here");
    }
}


#endif //NYX_INTERPRETER_H
