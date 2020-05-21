#ifndef NYX_AST_H
#define NYX_AST_H

#include <deque>

#include "../runtime/Global.h"
#include "Token.h"
#include <map>
#include <string>
#include <vector>

//===----------------------------------------------------------------------===//
// Ast node visitor, we can derive this class when needed
//===----------------------------------------------------------------------===//
struct Expr;
struct Stmt;
struct Block;
struct BoolExpr;
struct CharExpr;
struct NullExpr;
struct IntExpr;
struct Expr;
struct DoubleExpr;
struct StringExpr;
struct ArrayExpr;
struct IdentExpr;
struct IndexExpr;
struct BinaryExpr;
struct FuncCallExpr;
struct AssignExpr;
struct ClosureExpr;
struct Stmt;
struct BreakStmt;
struct ContinueStmt;
struct SimpleStmt;
struct ReturnStmt;
struct IfStmt;
struct WhileStmt;
struct ForStmt;
struct ForEachStmt;
struct MatchStmt;
struct ImportStmt;
struct ExportStmt;
struct Block;
struct FuncDef;
struct CompilationUnit;

struct AstVisitor {
    virtual void visitBlock(Block* node) {}

    virtual void visitFuncDef(FuncDef* node) {}

    virtual void visitCompilationUnit(CompilationUnit* node) {}

    virtual void visitBoolExpr(BoolExpr* node) {}

    virtual void visitCharExpr(CharExpr* node) {}

    virtual void visitNullExpr(NullExpr* node) {}

    virtual void visitIntExpr(IntExpr* node) {}

    virtual void visitExpr(Expr* expr) {}

    virtual void visitDoubleExpr(DoubleExpr* node) {}

    virtual void visitStringExpr(StringExpr* node) {}

    virtual void visitArrayExpr(ArrayExpr* node) {}

    virtual void visitIdentExpr(IdentExpr* node) {}

    virtual void visitIndexExpr(IndexExpr* node) {}

    virtual void visitBinaryExpr(BinaryExpr* node) {}

    virtual void visitFuncCallExpr(FuncCallExpr* node) {}

    virtual void visitAssignExpr(AssignExpr* node) {}

    virtual void visitClosureExpr(ClosureExpr* node) {}

    virtual void visitStmt(Stmt* node) {}

    virtual void visitBreakStmt(BreakStmt* node) {}

    virtual void visitContinueStmt(ContinueStmt* node) {}

    virtual void visitSimpleStmt(SimpleStmt* node) {}

    virtual void visitReturnStmt(ReturnStmt* node) {}

    virtual void visitIfStmt(IfStmt* node) {}

    virtual void visitWhileStmt(WhileStmt* node) {}

    virtual void visitForStmt(ForStmt* node) {}

    virtual void visitForEachStmt(ForEachStmt* node) {}

    virtual void visitMatchStmt(MatchStmt* node) {}

    virtual void visitImportStmt(ImportStmt* node) {}

    virtual void visitExportStmt(ExportStmt* node) {}
};

//===----------------------------------------------------------------------===//
// Top level ast nodes
//===----------------------------------------------------------------------===//
struct AstNode {
    explicit AstNode(int line, int column);

    virtual ~AstNode() = default;

    int line = -1;
    int column = -1;

    int id = 0;

    virtual void visit(AstVisitor* visitor) {}

    virtual std::string to_string() const;

    int getId() const { return id; }
};

#define AST_ROOT(root_name)                                                    \
    struct root_name : public AstNode {                                        \
        using AstNode::AstNode;                                                \
        void visit(AstVisitor* visitor) override {                             \
            visitor->visit##root_name(this);                                   \
        }                                                                      \
        std::string to_string() const override;                                \
        virtual ~root_name();

#define AST_ROOT_END                                                           \
    }                                                                          \
    ;

AST_ROOT(Expr)
AST_ROOT_END

AST_ROOT(Stmt)
AST_ROOT_END

AST_ROOT(Block)

std::vector<Stmt*> stmts;
AST_ROOT_END

AST_ROOT(FuncDef)

std::string funcName;
std::vector<std::string> params;
Block* block{};
AST_ROOT_END

AST_ROOT(CompilationUnit)

explicit CompilationUnit() : AstNode(-1, -1) {}

std::vector<FuncDef*> definitions;
std::vector<ImportStmt*> imports;
std::vector<ExportStmt*> exports;
std::vector<Stmt*> topStmts;
AST_ROOT_END

//===----------------------------------------------------------------------===//
// Expressions
//===----------------------------------------------------------------------===//
#define AST_EXPR(expr_name)                                                    \
    struct expr_name : public Expr {                                           \
        using Expr::Expr;                                                      \
        void visit(AstVisitor* visitor) override {                             \
            visitor->visit##expr_name(this);                                   \
        }                                                                      \
        std::string to_string() const override;                                \
        virtual ~expr_name();

#define AST_EXPR_END                                                           \
    }                                                                          \
    ;

AST_EXPR(BoolExpr)

bool literal;
AST_EXPR_END

AST_EXPR(CharExpr)

nyx::int8 literal;
AST_EXPR_END

AST_EXPR(NullExpr)
AST_EXPR_END

AST_EXPR(IntExpr)

nyx::int32 literal;
AST_EXPR_END

AST_EXPR(DoubleExpr)

double literal;
AST_EXPR_END

AST_EXPR(StringExpr)

std::string literal;
AST_EXPR_END

AST_EXPR(ArrayExpr)

std::vector<Expr*> literal;
AST_EXPR_END

AST_EXPR(IdentExpr)

std::string identName;
AST_EXPR_END

AST_EXPR(IndexExpr)

std::string identName;
Expr* index{};
AST_EXPR_END

AST_EXPR(BinaryExpr)

Expr* lhs{};
Token opt{};
Expr* rhs{};
AST_EXPR_END

AST_EXPR(FuncCallExpr)

std::string moduleName;
std::string funcName;
ClosureExpr* closure{};
std::vector<Expr*> args;
AST_EXPR_END

AST_EXPR(AssignExpr)

Expr* lhs{};
Token opt;
Expr* rhs{};
AST_EXPR_END

AST_EXPR(ClosureExpr)

std::vector<std::string> params;
Block* block{};
AST_EXPR_END

//===----------------------------------------------------------------------===//
// Statements
//===----------------------------------------------------------------------===//
#define AST_STMT(stmt_name)                                                    \
    struct stmt_name : public Stmt {                                           \
        using Stmt::Stmt;                                                      \
        void visit(AstVisitor* visitor) override {                             \
            visitor->visit##stmt_name(this);                                   \
        }                                                                      \
        std::string to_string() const override {                               \
            return std::move(std::string("" #stmt_name));                      \
        }                                                                      \
        virtual ~stmt_name();

#define AST_STMT_END                                                           \
    }                                                                          \
    ;

AST_STMT(BreakStmt)
AST_STMT_END

AST_STMT(ContinueStmt)
AST_STMT_END

AST_STMT(SimpleStmt)

Expr* expr{};
AST_STMT_END

AST_STMT(ReturnStmt)

Expr* retval{};
AST_STMT_END

AST_STMT(IfStmt)

Expr* cond{};
Block* block{};
Block* elseBlock{};
AST_STMT_END

AST_STMT(WhileStmt)

Expr* cond{};
Block* block{};
AST_STMT_END

AST_STMT(ForStmt)

Expr* init{};
Expr* cond{};
Expr* post{};
Block* block{};
AST_STMT_END

AST_STMT(ForEachStmt)

std::string identName;
Expr* list{};
Block* block{};
AST_STMT_END

AST_STMT(MatchStmt)

Expr* cond{};
std::vector<std::tuple<Expr*, Block*, bool>> matches;
AST_STMT_END

AST_STMT(ImportStmt)

std::vector<IdentExpr*> imports;
AST_STMT_END

AST_STMT(ExportStmt)

std::vector<IdentExpr*> exports;
AST_STMT_END

#endif