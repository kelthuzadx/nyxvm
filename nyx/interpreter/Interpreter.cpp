#include <iostream>
#include "Interpreter.h"
#include "../bytecode/Opcode.h"

Interpreter::Interpreter(MetaArea* meta) :
       meta(meta),bytecodes(meta->bytecodes) {
    bci = 0;
}

void Interpreter::execute() {
    for(int i=0;i<meta->bytecodeSize;i++){
        switch (bytecodes[i]){
            case JMP:
                std::cout<<"jmp "<<bytecodes[i+1]<<"\n";
                i++;
                break;
            case JMP_EQ:
                std::cout<<"jmp_eq "<<bytecodes[i+1]<<"\n";
                i++;
                break;
            case JMP_NE:
                std::cout<<"jmp_ne "<<bytecodes[i+1]<<"\n";
                i++;
                break;
            case CONST_I:
                std::cout<<"const_i "<<(int)*(bytecodes+1)<<"\n";
                i+=4;
                break;
            case CONST_NULL:
                std::cout<<"const_null\n";
                break;
            case CONST_D:
                std::cout<<"const_d "<<(double)*(bytecodes+1)<<"\n";
                i+=8;
                break;
            case CONST_STR:
                std::cout<<"const_str "<<meta->strings[bytecodes[i+1]]<<"\n";
                i++;
                break;
        }
    }
}