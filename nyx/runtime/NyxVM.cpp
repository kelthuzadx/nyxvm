#include "NyxVM.h"
#include "../bytecode/Bytecode.h"
#include "../bytecode/BytecodeDump.h"
#include "../bytecode/BytecodeGenerator.h"
#include "../gc/GenHeap.h"
#include "../interpreter/Interpreter.h"
#include "../parser/Ast.h"
#include "../parser/AstDump.h"
#include "../parser/Parser.h"
#include "Builtin.h"

void NyxVM::ignition(const char* script) {
    Parser::dumpLex("lex.log", script);
    Parser parser(script);
    CompilationUnit* unit = parser.parse();
    {
        AstDump astDumper("ast.dot");
        astDumper.dump(unit);
    }
    BytecodeGenerator gen;
    Bytecode* bytecode = gen.generate(unit);
    {
        BytecodeDump bcDumper("bytecode.log");
        bcDumper.dump(bytecode);
    }
    delete unit;
    Interpreter interpreter;
    interpreter.execute(bytecode);
    delete bytecode;
}

void NyxVM::setup() {
    PhaseTime timer("set up everything about VM");
    Opcode::initialize();
    GenHeap::initialize();
}
