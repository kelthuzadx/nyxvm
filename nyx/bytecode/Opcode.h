#ifndef NYX_OPCODE_H
#define NYX_OPCODE_H

enum Opcode {
    JMP,
    JMP_EQ,
    JMP_NE,
    CONST_I,
    CONST_NULL,
    CONST_D,
};


inline const char *getOpcodeSymbol(Token token) {
    static const char *symbol[] = {
            "jmp",
            "jmp_eq",
            "jmp_ne",
            "const_i",
            "const_null",
            "const_d",

    };
    return symbol[token];
}

#endif //NYX_OPCODE_H
