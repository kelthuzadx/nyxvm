#ifndef NYX_ASTDUMP_H
#define NYX_ASTDUMP_H

#include "Ast.h"
#include <fstream>

//===----------------------------------------------------------------------===//
// Dump Ast to *.dot file, this file can be further visualized by Graphviz
//===----------------------------------------------------------------------===//
class AstDump : protected AstVisitor {
  private:
    std::ofstream ofs;
    PhaseTime timer;

    void addEdge(AstNode* from, AstNode* to);

  public:
    explicit AstDump(const std::string& dotFilePath);

    ~AstDump();

    void dump(CompilationUnit* unit);

  protected:
    void visitBoolExpr(BoolExpr* node) override;

    void visitCharExpr(CharExpr* node) override;

    void visitNullExpr(NullExpr* node) override;

    void visitIntExpr(IntExpr* node) override;

    void visitExpr(Expr* expr) override;

    void visitDoubleExpr(DoubleExpr* node) override;

    void visitStringExpr(StringExpr* node) override;

    void visitArrayExpr(ArrayExpr* node) override;

    void visitIdentExpr(IdentExpr* node) override;

    void visitIndexExpr(IndexExpr* node) override;

    void visitBinaryExpr(BinaryExpr* node) override;

    void visitFuncCallExpr(FuncCallExpr* node) override;

    void visitAssignExpr(AssignExpr* node) override;

    void visitClosureExpr(ClosureExpr* node) override;

    void visitStmt(Stmt* node) override;

    void visitBreakStmt(BreakStmt* node) override;

    void visitContinueStmt(ContinueStmt* node) override;

    void visitSimpleStmt(SimpleStmt* node) override;

    void visitReturnStmt(ReturnStmt* node) override;

    void visitIfStmt(IfStmt* node) override;

    void visitWhileStmt(WhileStmt* node) override;

    void visitForStmt(ForStmt* node) override;

    void visitForEachStmt(ForEachStmt* node) override;

    void visitMatchStmt(MatchStmt* node) override;

    void visitImportStmt(ImportStmt* node) override;

    void visitExportStmt(ExportStmt* node) override;

    void visitBlock(Block* node) override;

    void visitFuncDef(FuncDef* node) override;

    void visitCompilationUnit(CompilationUnit* node) override;
};

#endif // NYX_ASTDUMP_H
