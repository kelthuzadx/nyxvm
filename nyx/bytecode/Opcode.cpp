#include "Opcode.h"
#include "../util/Utils.h"
#include <cstring>

const char* Opcode::name[Opcode::num_of_mnemonic];
int Opcode::format[Opcode::num_of_mnemonic];

void Opcode::initialize() {
    defineOpcode(CALL, "call", 1);
    defineOpcode(JMP, "jmp", 1);
    defineOpcode(JMP_EQ, "jmp_eq", 1);
    defineOpcode(JMP_NE, "jmp_ne", 1);
    defineOpcode(CONST_I, "const_i", 4);
    defineOpcode(CONST_NULL, "const_null", 0);
    defineOpcode(CONST_D, "const_d", 8);
    defineOpcode(CONST_C, "const_c", 1);
    defineOpcode(CONST_STR, "const_str", 1);
    defineOpcode(CONST_CLOSURE, "const_closure", 1);
    defineOpcode(NEG, "neg", 0);
    defineOpcode(TEST_EQ, "test_eq", 0);
    defineOpcode(TEST_NE, "test_ne", 0);
    defineOpcode(TEST_GE, "test_ge", 0);
    defineOpcode(TEST_GT, "test_gt", 0);
    defineOpcode(TEST_LE, "test_le", 0);
    defineOpcode(TEST_LT, "test_lt", 0);
    defineOpcode(AND, "and", 0);
    defineOpcode(OR, "or", 0);
    defineOpcode(NOT, "not", 0);
    defineOpcode(ADD, "add", 0);
    defineOpcode(SUB, "sub", 0);
    defineOpcode(MUL, "mul", 0);
    defineOpcode(DIV, "div", 0);
    defineOpcode(REM, "rem", 0);
    defineOpcode(RETURN, "return", 0);
    defineOpcode(RETURN_VAL, "return_val", 0);
    defineOpcode(LOAD, "load", 1);
    defineOpcode(STORE, "store", 1);
    defineOpcode(LOAD_INDEX, "load_index", 0);
    defineOpcode(STORE_INDEX, "store_index", 0);
    defineOpcode(NEW_ARR, "new_arr", 1);
    defineOpcode(DUP, "dup", 0);
    defineOpcode(ARR_LEN, "arr_len", 0);
    defineOpcode(LOAD_FREE, "load_free", 1);
    defineOpcode(STORE_FREE, "store_free", 1);

    verify();
}

void Opcode::defineOpcode(Opcode::Mnemonic m, const char* n, int f) {
    name[m] = n;
    format[m] = f;
}

void Opcode::verify() {
    for (auto& i : name) {
        if (strlen(i) == 0) {
            panic("incomplete opcode information");
        }
    }
}

const char* Opcode::forName(int mnemonic) { return name[mnemonic]; }

int Opcode::forFormat(int mnemonic) { return format[mnemonic]; }
