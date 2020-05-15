#include <iostream>
#include "BytecodeGenerator.h"
#include "Opcode.h"

void BytecodeGenerator::visitBlock(Block *node) {}

void BytecodeGenerator::visitFuncDef(FuncDef *node) {}

void BytecodeGenerator::visitCompilationUnit(CompilationUnit *node) {
    for (auto *stmt:node->topStmts) {
        stmt->visit(this);
    }
}

void BytecodeGenerator::visitBoolExpr(BoolExpr *node) {
    bytecodes[bci++] = CONST_I;
    bytecodes[bci++] = (int)node->literal;
}

void BytecodeGenerator::visitCharExpr(CharExpr *node) {
    bytecodes[bci++] = CONST_I;
    bytecodes[bci++] = (int)node->literal;
}

void BytecodeGenerator::visitNullExpr(NullExpr *node) {
    bytecodes[bci++] = CONST_NULL;
}

void BytecodeGenerator::visitIntExpr(IntExpr *node) {
    bytecodes[bci++] = CONST_I;
    bytecodes[bci++] = (int)node->literal;
}

void BytecodeGenerator::visitExpr(Expr *expr) {
    panic("should not reach here");
}

void BytecodeGenerator::visitDoubleExpr(DoubleExpr *node) {
    bytecodes[bci++] = CONST_I;
    bytecodes[bci++]  = node->literal;
}

void BytecodeGenerator::visitStringExpr(StringExpr *node) {}

void BytecodeGenerator::visitArrayExpr(ArrayExpr *node) {}

void BytecodeGenerator::visitIdentExpr(IdentExpr *node) {}

void BytecodeGenerator::visitIndexExpr(IndexExpr *node) {}

void BytecodeGenerator::visitBinaryExpr(BinaryExpr *node) {}

void BytecodeGenerator::visitFuncCallExpr(FuncCallExpr *node) {}

void BytecodeGenerator::visitAssignExpr(AssignExpr *node) {}

void BytecodeGenerator::visitClosureExpr(ClosureExpr *node) {}

void BytecodeGenerator::visitStmt(Stmt *node) {
    panic("should not reach here");
}

void BytecodeGenerator::visitBreakStmt(BreakStmt *node) {}

void BytecodeGenerator::visitContinueStmt(ContinueStmt *node) {}

void BytecodeGenerator::visitSimpleStmt(SimpleStmt *node) {}

void BytecodeGenerator::visitReturnStmt(ReturnStmt *node) {}

void BytecodeGenerator::visitIfStmt(IfStmt *node) {
    node->cond->visit(this);
    if (node->elseBlock == nullptr) {
        // if(cond){ ... }
        // COND
        // JMP_NE Target
        // ... <then block>
        // Target:
        bytecodes[bci++] = JMP_NE;
        int patching = bci;
        bytecodes[bci++] = -1; // target, further patching
        std::cout << "jmp_eq -1";
        node->block->visit(this);
        bytecodes[patching] = bci;
    } else {
        // if(cond){ ... } else{ ... }
        // COND
        // JMP_NE Else
        // ... <then block>
        // JMP Target
        // Else:
        // ... <else block>
        // JMP Target
        // Target:
        bytecodes[bci++] = JMP_NE;
        int falsePatching = bci;
        bytecodes[bci++] = -1; // target, further patching
        node->block->visit(this);
        bytecodes[bci++] = JMP;
        int targetPatching = bci;
        bytecodes[bci++] = -1;
        bytecodes[falsePatching] = bci;
        node->elseBlock->visit(this);
        bytecodes[bci++] = JMP;
        bytecodes[bci++] = bci + 1;
        bytecodes[targetPatching] = bci;
    }
}

void BytecodeGenerator::visitWhileStmt(WhileStmt *node) {}

void BytecodeGenerator::visitForStmt(ForStmt *node) {}

void BytecodeGenerator::visitForEachStmt(ForEachStmt *node) {}

void BytecodeGenerator::visitMatchStmt(MatchStmt *node) {}

void BytecodeGenerator::visitImportStmt(ImportStmt *node) {}

void BytecodeGenerator::visitExportStmt(ExportStmt *node) {}

