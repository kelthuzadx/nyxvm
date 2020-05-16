#include "util/Utils.hpp"
#include "parser/Parser.h"
#include "parser/AstDump.h"
#include "bytecode/BytecodeGenerator.h"
#include "interpreter/Interpreter.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        panic("Feed your *.nyx source file to interpreter!\n");
    }

    nyx::Parser parser(argv[1]);
    CompilationUnit *unit = parser.parse();
    AstDump dumper("ast.dot");
    dumper.dump(unit);
    BytecodeGenerator gen(unit);
    MetaArea *meta = gen.generate();
    Interpreter interpreter(meta);
    interpreter.execute();
    return 0;
}
