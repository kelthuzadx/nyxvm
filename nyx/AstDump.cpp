#include "AstDump.h"

AstDump::AstDump(const std::string &dotFilePath)
        : ofs(dotFilePath, std::ios::out) {

}

void AstDump::addEdge(AstNode *from, AstNode *to) {
    ofs << "\tnode_" << from->name() << "[label=\"" << from->name() << "\"]";
}

void AstDump::dump(CompilationUnit *unit) {
    ofs << "digraph G{\n";
    ofs << "graph [ dpi = 300 ]\\n";
    unit->visit(this);
    ofs << "}";
    ofs.flush();
}

void AstDump::visitCompilationUnit(CompilationUnit *node) {
    for (FuncDef *def:node->definitions) {
        addEdge(node, def);
        def->visit(this);
    }
    for (Stmt *stmt:node->topStmts) {
        addEdge(node, stmt);
        stmt->visit(this);
    }
}



