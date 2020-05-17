#ifndef NYX_BYTECODEGENERATOR_H
#define NYX_BYTECODEGENERATOR_H

#include <cstdint>
#include <unordered_map>
#include "../parser/Ast.h"
#include "../util/Utils.h"
#include "Bytecode.h"
#include "Opcode.h"

class BytecodeGenerator : public AstVisitor {
    friend class Label;

    friend class Jmp;

private:
    std::unordered_map<std::string, int> localMap;
    Bytecode *bytecode{};
    int bci;
    int local;

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

private:
    Bytecode *generate(FuncDef *node);

    void fixupBytecode(const std::string &funcName);

    void constInt(nyx::int32 integer);

    void varLoad(int localIndex);

    void varStore(int localIndex);

    bool isShortCircuitAnd(Expr *expr);

    bool isShortCircuitOr(Expr *expr);


public:
    explicit BytecodeGenerator();

    Bytecode *generate(CompilationUnit *unit);
};


#endif //NYX_BYTECODEGENERATOR_H
