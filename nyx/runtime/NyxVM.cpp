#include "NyxVM.h"
#include "Builtin.h"
#include "../parser/Ast.h"
#include "../parser/AstDump.h"
#include "../bytecode/BytecodeGenerator.h"
#include "../bytecode/MetaArea.h"
#include "../interpreter/Interpreter.h"
#include "../parser/Parser.h"


const char *((NyxVM::builtin[])[2]) = {
        {"nyxffi_println", (const char *) nyxffi_println}
};

void NyxVM::ignition(const char *script) {
    nyx::Parser parser(script);
    CompilationUnit *unit = parser.parse();
    AstDump dumper("ast.dot");
    dumper.dump(unit);
    BytecodeGenerator gen(unit);
    MetaArea *meta = gen.generate();
    Interpreter interpreter(meta);
    interpreter.execute();
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
}






