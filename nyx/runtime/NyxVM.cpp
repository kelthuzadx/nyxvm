#include "NyxVM.h"
#include "Builtin.h"
#include "../parser/Ast.h"
#include "../parser/AstDump.h"
#include "../bytecode/BytecodeGenerator.h"
#include "../bytecode/Bytecode.h"
#include "../interpreter/Interpreter.h"
#include "../parser/Parser.h"
#include "../bytecode/BytecodeDump.h"


const char *((NyxVM::builtin[])[2]) = {
        {"nyxffi_println", (const char *) nyxffi_println},
        {"nyxffi_print",   (const char *) nyxffi_print},
        {"nyxffi_typeof",  (const char *) nyxffi_typeof},
        {"nyxffi_len",     (const char *) nyxffi_len},
        {"nyxffi_exit",    (const char *) nyxffi_exit},
        {"nyxffi_assert",  (const char *) nyxffi_assert}
};

void NyxVM::ignition(const char *script) {
    nyx::Parser parser(script);
    CompilationUnit *unit = parser.parse();
    AstDump astDumper("ast.dot");
    astDumper.dump(unit);
    BytecodeGenerator gen;
    Bytecode *bytecode = gen.generate(unit);
    BytecodeDump bcDumper("bytecode.log");
    bcDumper.dump(bytecode);
    delete unit;
    Interpreter interpreter;
    interpreter.execute(bytecode);
    delete bytecode;
}

const char *NyxVM::findBuiltin(const std::string &name) {
    std::string target = "nyxffi_" + name;
    for (auto &func : builtin) {
        const char *builtinFuncName = func[0];
        if (target == builtinFuncName) {
            return func[1];
        }
    }
    return nullptr;
}

void NyxVM::setup() {
    PhaseTime timer("set up everything about VM");
    Opcode::initialize();
}






