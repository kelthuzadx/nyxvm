#include "BytecodeDump.h"
#include "Bytecode.h"
#include "Opcode.h"

BytecodeDump::BytecodeDump(const std::string &logPath) : timer("dump bytecode") {
    ofs.open(logPath, std::ios::out);
}

BytecodeDump::~BytecodeDump() {
    ofs.flush();
    ofs.close();
}

void BytecodeDump::dump(Bytecode *bytecode) {
    ofs << "=====" << bytecode->funcName << "=====\n";
    int bytecodeSize = bytecode->codeSize;
    auto *bytecodes = bytecode->code;
    for (int bci = 0; bci < bytecodeSize; bci++) {
        switch (bytecodes[bci]) {
            case Opcode::CALL: {
                int funcArgc = bytecodes[bci + 1];
                ofs << bci << ":" << "call " << funcArgc << "\n";
                bci++;
                break;
            }
            case Opcode::CONST_I: {
                nyx::int32 value = *(nyx::int32 *) (bytecodes + bci + 1);
                ofs << bci << ":" << "const_i " << value << "\n";
                bci += 4;
                break;
            }
            case Opcode::CONST_D: {
                double value = *(double *) (bytecodes + bci + 1);
                ofs << bci << ":" << "const_d " << value << "\n";
                bci += 8;
                break;
            }
            case Opcode::CONST_NULL: {
                ofs << bci << ":" << "const_null" << "\n";
                break;
            }
            case Opcode::CONST_STR: {
                int index = bytecodes[bci + 1];
                auto &str = bytecode->strings[index];
                ofs << bci << ":" << "const_str '" << str << "'\n";
                bci++;
                break;
            }
            case Opcode::ADD: {
                ofs << bci << ":" << "add" << "\n";
                break;
            }
            case Opcode::SUB: {
                ofs << bci << ":" << "sub" << "\n";
                break;
            }
            case Opcode::MUL: {
                ofs << bci << ":" << "mul" << "\n";
                break;
            }
            case Opcode::DIV: {
                ofs << bci << ":" << "div" << "\n";
                break;
            }
            case Opcode::REM: {
                ofs << bci << ":" << "rem" << "\n";
                break;
            }
            case Opcode::TEST_EQ: {
                ofs << bci << ":" << "test_eq" << "\n";
                break;
            }
            case Opcode::TEST_NE: {
                ofs << bci << ":" << "test_ne" << "\n";
                break;
            }
            case Opcode::TEST_GE: {
                ofs << bci << ":" << "test_ge" << "\n";
                break;
            }
            case Opcode::TEST_GT: {
                ofs << bci << ":" << "test_gt" << "\n";
                break;
            }
            case Opcode::TEST_LE: {
                ofs << bci << ":" << "test_le" << "\n";
                break;
            }
            case Opcode::TEST_LT: {
                ofs << bci << ":" << "test_lt" << "\n";
                break;
            }
            case Opcode::JMP: {
                int target = bytecodes[bci + 1];
                ofs << bci << ":" << "jmp " << target << "\n";
                bci++;
                break;
            }
            case Opcode::JMP_NE: {
                int target = bytecodes[bci + 1];
                ofs << bci << ":" << "jmp_ne " << target << "\n";
                bci++;
                break;
            }
            case Opcode::JMP_EQ: {
                int target = bytecodes[bci + 1];
                ofs << bci << ":" << "jmp_eq " << target << "\n";
                bci++;
                break;
            }
            case Opcode::AND: {
                ofs << bci << ":" << "and" << "\n";
                break;
            }
            case Opcode::OR: {
                ofs << bci << ":" << "or" << "\n";
                break;
            }
            case Opcode::NOT: {
                ofs << bci << ":" << "not" << "\n";
                break;
            }
            case Opcode::NEG: {
                ofs << bci << ":" << "neg" << "\n";
                break;
            }
            case Opcode::LOAD: {
                int index = bytecodes[bci + 1];
                ofs << bci << ":" << "load " << index << "\n";
                bci++;
                break;
            }
            case Opcode::STORE: {
                int index = bytecodes[bci + 1];
                ofs << bci << ":" << "store " << index << "\n";
                bci++;
                break;
            }
            case Opcode::LOAD_INDEX: {
                ofs << bci << ":" << "load_index" << "\n";
                break;
            }
            case Opcode::STORE_INDEX: {
                ofs << bci << ":" << "store_index" << "\n";
                break;
            }
            case Opcode::NEW_ARR: {
                int length = bytecodes[bci + 1];
                ofs << bci << ":" << "new_array " << length << "\n";
                bci++;
                break;
            }
            case Opcode::DUP: {
                ofs << bci << ":" << "dup" << "\n";
                break;
            }
            case Opcode::RETURN: {
                ofs << bci << ":" << "return" << "\n";
                break;
            }
            case Opcode::RETURN_VAL: {
                ofs << bci << ":" << "return_val" << "\n";
                break;
            }
            case Opcode::ARR_LEN: {
                ofs << bci << ":" << "arr_len" << "\n";
                break;
            }
            case Opcode::LOAD_FREE: {
                int index = bytecodes[bci + 1];
                ofs << bci << ":" << "load_free " << index << "\n";
                bci++;
                break;
            }
            case Opcode::CONST_CLOSURE: {
                int index = bytecodes[bci + 1];
                ofs << bci << ":" << "const_closure " << index << "\n";
                bci++;
                break;
            }
            default:
                ofs << bci << ":" << "<illegal>" << "\n";
                break;
        }
    }

    for (auto &func:bytecode->functions) {
        dump(func.second);
    }

    for (auto &func:bytecode->closures) {
        dump(func.second);
    }
    ofs.flush();
}
