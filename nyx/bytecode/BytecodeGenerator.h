#ifndef NYX_BYTECODEGENERATOR_H
#define NYX_BYTECODEGENERATOR_H

#include <cstdint>
#include "../parser/Ast.h"
#include "../runtime/Utils.hpp"
#include "MetaArea.h"

class BytecodeGenerator : public AstVisitor {
private:
    CompilationUnit *unit;
    MetaArea *meta{};
    int bci;

private:
    void visitBlock(Block *node) override;

    void visitFuncDef(FuncDef *node) override;

    void visitCompilationUnit(CompilationUnit *node) override;

    void visitBoolExpr(BoolExpr *node) override;

    void visitCharExpr(CharExpr *node) override;

    void visitNullExpr(NullExpr *node) override;

    void visitIntExpr(IntExpr *node) override;

    void visitExpr(Expr *expr) override;

    void visitDoubleExpr(DoubleExpr *node) override;

    void visitStringExpr(StringExpr *node) override;

    void visitArrayExpr(ArrayExpr *node) override;

    void visitIdentExpr(IdentExpr *node) override;

    void visitIndexExpr(IndexExpr *node) override;

    void visitBinaryExpr(BinaryExpr *node) override;

    void visitFuncCallExpr(FuncCallExpr *node) override;

    void visitAssignExpr(AssignExpr *node) override;

    void visitClosureExpr(ClosureExpr *node) override;

    void visitStmt(Stmt *node) override;

    void visitBreakStmt(BreakStmt *node) override;

    void visitContinueStmt(ContinueStmt *node) override;

    void visitSimpleStmt(SimpleStmt *node) override;

    void visitReturnStmt(ReturnStmt *node) override;

    void visitIfStmt(IfStmt *node) override;

    void visitWhileStmt(WhileStmt *node) override;

    void visitForStmt(ForStmt *node) override;

    void visitForEachStmt(ForEachStmt *node) override;

    void visitMatchStmt(MatchStmt *node) override;

    void visitImportStmt(ImportStmt *node) override;

    void visitExportStmt(ExportStmt *node) override;

public:
    explicit BytecodeGenerator(CompilationUnit *unit) : unit(unit) {
        meta = new MetaArea;
        meta->bytecodes = new nyx::int8[65535];
        meta->bytecodeSize = -1;
        bci = 0;
    }

    MetaArea *generate() {
        PhaseTime timer("generate bytecode from Ast");
        unit->visit(this);
        meta->bytecodeSize = bci;
        return meta;
    }
};


#endif //NYX_BYTECODEGENERATOR_H
