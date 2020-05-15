#include <iostream>
#include "Interpreter.h"
#include "../bytecode/Opcode.h"

Interpreter::Interpreter(MetaArea* meta) :
       meta(meta){
}

void Interpreter::execute() {
    frame = new Frame;
    stack.push_back(frame);

    const int bytecodeSize = meta->bytecodeSize;
    const nyx::int8* bytecodes =meta->bytecodes;
    int bci = 0;

    PhaseTime timer("execute bytecode");
    for(int i=0;i<bytecodeSize;i++){
        switch (bytecodes[i]){
            case CALL:{
                int funcNameIndex = bytecodes[i+1];
                std::string str = meta->strings[funcNameIndex];
                break;
            }
            case CONST_I: {
                auto *object = new NInt(bytecodes[i + 1]);
                frame->slots.push_back(object);
                i++;
                break;
            }
            case ADD: {
                Object *object1 = frame->slots.back();
                frame->slots.pop_back();
                Object *object2 = frame->slots.back();
                frame->slots.pop_back();
                break;
            }
            default:
                panic("should not reach here");
        }
    }
}