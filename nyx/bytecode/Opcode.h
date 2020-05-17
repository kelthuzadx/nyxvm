#ifndef NYX_OPCODE_H
#define NYX_OPCODE_H

enum Opcode {
    NOP,
    CALL,
    JMP,
    JMP_EQ,
    JMP_NE,
    CONST_I,
    CONST_NULL,
    CONST_D,
    CONST_STR,
    NEG,
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
    LOAD,
    STORE,
    LOAD_INDEX,
    STORE_INDEX,
};

#endif //NYX_OPCODE_H
