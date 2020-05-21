#ifndef NYX_BYTECODEGENERATOR_H
#define NYX_BYTECODEGENERATOR_H

#include "../parser/Ast.h"
#include "../util/Utils.h"
#include "Bytecode.h"
#include "Opcode.h"
#include <cstdint>
#include <unordered_map>

class Label;
//===----------------------------------------------------------------------===//
// Generate bytecode from Ast
//===----------------------------------------------------------------------===//
class BytecodeGenerator : public AstVisitor {
    friend class Label;

    friend class Jmp;

  private:
    Bytecode* bytecode{};
    Label* continuePoint{};
    Label* breakPoint{};
    int bci;

  private:
    void visitBlock(Block* node) override;

    void visitFuncDef(FuncDef* node) override;

    void visitCompilationUnit(CompilationUnit* node) override;

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

  private:
    Bytecode* generateFuncDef(FuncDef* node);

    Bytecode* generateClosureExpr(Bytecode* enclosing, ClosureExpr* node);

    void genConstI(nyx::int32 integer);

    void genConstStr(const std::string& str);

    void genConstNull();

    void genConstD(double d);

    void genLoad(const std::string& name);

    void genStore(const std::string& name);

    void genLoadIndex(const std::string& array, Expr* index);

    void genStoreIndex(const std::string& array, Expr* value, Expr* index);

    void genArray(std::vector<Expr*> node);

    static bool isShortCircuitAnd(Expr* expr);

    static bool isShortCircuitOr(Expr* expr);

  public:
    explicit BytecodeGenerator();

    Bytecode* generate(CompilationUnit* unit);
};

#endif // NYX_BYTECODEGENERATOR_H
