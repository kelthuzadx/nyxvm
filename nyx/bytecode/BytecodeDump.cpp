#include "BytecodeDump.h"
#include "Bytecode.h"
#include "Opcode.h"

BytecodeDump::BytecodeDump(const std::string& logPath)
    : timer("dump bytecode") {
    ofs.open(logPath, std::ios::out);
}

BytecodeDump::~BytecodeDump() {
    ofs.flush();
    ofs.close();
}

void BytecodeDump::dump(Bytecode* bytecode) {
    ofs << ";; " << bytecode->funcName << "\n";
    ofs << ";; {";
    for (auto& iter : bytecode->localVars) {
        ofs << iter.second << "=" << iter.first << ",";
    }
    ofs << "}\n";
    int bytecodeSize = bytecode->codeSize;
    auto* bytecodes = bytecode->code;
    for (int bci = 0; bci < bytecodeSize; bci++) {
        switch (bytecodes[bci]) {
        case Opcode::CONST_I: {
            nyx::int32 value = *(nyx::int32*)(bytecodes + bci + 1);
            ofs << bci << ":"
                << "const_i " << value << "\n";
            bci += 4;
            break;
        }
        case Opcode::CONST_D: {
            double value = *(double*)(bytecodes + bci + 1);
            ofs << bci << ":"
                << "const_d " << value << "\n";
            bci += 8;
            break;
        }
        case Opcode::CONST_STR: {
            int index = bytecodes[bci + 1];
            auto& str = bytecode->strings[index];
            ofs << bci << ":"
                << "const_str '" << str << "'\n";
            bci++;
            break;
        }
        default:
            ofs << bci << ":" << Opcode::forName(bytecodes[bci]);
            int format = Opcode::forFormat(bytecodes[bci]);
            if (format != 0) {
                for (int i = 0; i < format; i++) {
                    ofs << " " << bytecodes[bci + format];
                }
                bci += format;
            }
            ofs << "\n";
            break;
        }
    }

    for (auto& func : bytecode->functions) {
        dump(func.second);
    }

    for (auto& func : bytecode->closures) {
        dump(func.second);
    }
    ofs.flush();
}
