#include "AstDump.h"

AstDump::AstDump(const std::string &dotFilePath) {
    ofs.open(dotFilePath, std::ios::out);
}

AstDump::~AstDump() {
    ofs.flush();
    ofs.close();
}


void AstDump::addEdge(AstNode *from, AstNode *to) {
    ofs << "\tnode_" << from->getId() << "[label=\"" << from->to_string() << "\"]\n";
    ofs << "\tnode_" << to->getId() << "[label=\"" << to->to_string() << "\"]\n";
    ofs << "\tnode_" << from->getId() << "-> node_" << to->getId() << "\n";
}

void AstDump::dump(CompilationUnit *unit) {
    PhaseTime timer("dump Ast to *.dot file");
    ofs << "digraph G{\n";
    ofs << "\tgraph [ dpi = 300 ]\n";
    unit->visit(this);
    ofs << "}";
    ofs.flush();
}

#define VISIT_FIELD(field_name)\
    if(node->field_name!=nullptr){\
        addEdge(node,node->field_name);\
        node->field_name->visit(this);\
    }

#define VISIT_VEC_FIELD(field_name)\
    for(auto* val:node->field_name){\
        addEdge(node,val);\
        val->visit(this);\
    }

void AstDump::visitCompilationUnit(CompilationUnit *node) {
    VISIT_VEC_FIELD(definitions);
    VISIT_VEC_FIELD(topStmts);
}

void AstDump::visitFuncDef(FuncDef *node) {
    VISIT_FIELD(block);
}

void AstDump::visitMatchStmt(MatchStmt *node) {
    VISIT_FIELD(cond);
    // Special handling
    for (auto&[expr, block, matchAll]:node->matches) {
        addEdge(node, expr);
        expr->visit(this);
        addEdge(node, block);
        block->visit(this);
    }
}

void AstDump::visitBlock(Block *node) {
    VISIT_VEC_FIELD(stmts);
}

void AstDump::visitForEachStmt(ForEachStmt *node) {
    VISIT_FIELD(list);
    VISIT_FIELD(block);
}

void AstDump::visitForStmt(ForStmt *node) {
    VISIT_FIELD(init);
    VISIT_FIELD(cond);
    VISIT_FIELD(post);
    VISIT_FIELD(block);
}

void AstDump::visitWhileStmt(WhileStmt *node) {
    VISIT_FIELD(cond);
    VISIT_FIELD(block);
}

void AstDump::visitIfStmt(IfStmt *node) {
    VISIT_FIELD(cond);
    VISIT_FIELD(block);
    VISIT_FIELD(elseBlock);
}

void AstDump::visitReturnStmt(ReturnStmt *node) {
    VISIT_FIELD(retval);
}

void AstDump::visitSimpleStmt(SimpleStmt *node) {
    VISIT_FIELD(expr);
}

void AstDump::visitContinueStmt(ContinueStmt *node) {
    // Pass
}

void AstDump::visitBreakStmt(BreakStmt *node) {
    // Pass
}


void AstDump::visitStmt(Stmt *node) {
    panic("should not reach here");
}

void AstDump::visitClosureExpr(ClosureExpr *node) {
    VISIT_FIELD(block);
}

void AstDump::visitAssignExpr(AssignExpr *node) {
    VISIT_FIELD(lhs);
    VISIT_FIELD(rhs);
}

void AstDump::visitFuncCallExpr(FuncCallExpr *node) {
    VISIT_VEC_FIELD(args);
    VISIT_FIELD(closure)
}

void AstDump::visitBinaryExpr(BinaryExpr *node) {
    VISIT_FIELD(lhs);
    VISIT_FIELD(rhs);
}

void AstDump::visitIndexExpr(IndexExpr *node) {
    VISIT_FIELD(index)
}

void AstDump::visitIdentExpr(IdentExpr *node) {
    // Pass
}

void AstDump::visitArrayExpr(ArrayExpr *node) {
    VISIT_VEC_FIELD(literal);
}

void AstDump::visitStringExpr(StringExpr *node) {
    //Pass
}

void AstDump::visitDoubleExpr(DoubleExpr *node) {
    // Pass
}

void AstDump::visitExpr(Expr *expr) {
    // Pass
}

void AstDump::visitIntExpr(IntExpr *node) {
    // Pass
}

void AstDump::visitNullExpr(NullExpr *node) {
    // Pass
}

void AstDump::visitCharExpr(CharExpr *node) {
    // Pass
}

void AstDump::visitBoolExpr(BoolExpr *node) {
    // Pass
}

void AstDump::visitExportStmt(ExportStmt *node) {
    VISIT_VEC_FIELD(exports);
}

void AstDump::visitImportStmt(ImportStmt *node) {
    VISIT_VEC_FIELD(imports);
}




