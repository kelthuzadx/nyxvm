#ifndef NYX_OPCODE_H
#define NYX_OPCODE_H

enum Opcode {
    JMP,
    CONST_I,
    CONST_NULL,
    CONST_D,
    CONST_STR,
    NEG,
    TEST,
    TEST_EQ,
    TEST_NE,
    TEST_GE,
    TEST_GT,
    TEST_LE,
    TEST_LT,
    AND,
    OR,
    NOT,
    ADD,
    SUB,
    MUL,
    DIV,
    REM,
    RETURN,
};


inline const char *getOpcodeSymbol(Opcode token) {
    static const char *symbol[] = {
            "jmp",
            "const_i",
            "const_null",
            "const_d",
            "const_str",
            "neg",
            "test",
            "test_eq",
            "test_ne",
            "test_ge",
            "test_gt",
            "test_le",
            "test_lt",
            "and",
            "or",
            "not",
            "return"

    };
    return symbol[token];
}

#endif //NYX_OPCODE_H
