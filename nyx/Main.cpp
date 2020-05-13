#include <string.h>
#include <iostream>
#include "Utils.hpp"
#include "Parser.h"
#include "AstDump.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        panic("Feed your *.nyx source file to interpreter!\n");
    }

    nyx::Parser parser(argv[1]);
    CompilationUnit *unit = parser.parse();
    AstDump dumper("ast.dot");
    dumper.dump(unit);
    return 0;
}
