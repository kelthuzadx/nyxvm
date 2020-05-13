#ifndef NYX_AST_H
#define NYX_AST_H

#include <deque>
#include <map>
#include <vector>
#include <string>
#include "Token.h"

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
struct Block;
struct FuncDef;
struct CompilationUnit;

struct AstVisitor {
    virtual void visitBoolExpr(BoolExpr *node) {}

    virtual void visitCharExpr(CharExpr *node) {}

    virtual void visitNullExpr(NullExpr *node) {}

    virtual void visitIntExpr(IntExpr *node) {}

    virtual void visitExpr(Expr *expr) {}

    virtual void visitDoubleExpr(DoubleExpr *node) {}

    virtual void visitStringExpr(StringExpr *node) {}

    virtual void visitArrayExpr(ArrayExpr *node) {}

    virtual void visitIdentExpr(IdentExpr *node) {}

    virtual void visitIndexExpr(IndexExpr *node) {}

    virtual void visitBinaryExpr(BinaryExpr *node) {}

    virtual void visitFuncCallExpr(FuncCallExpr *node) {}

    virtual void visitAssignExpr(AssignExpr *node) {}

    virtual void visitClosureExpr(ClosureExpr *node) {}

    virtual void visitStmt(Stmt *node) {}

    virtual void visitBreakStmt(BreakStmt *node) {}

    virtual void visitContinueStmt(ContinueStmt *node) {}

    virtual void visitSimpleStmt(SimpleStmt *node) {}

    virtual void visitReturnStmt(ReturnStmt *node) {}

    virtual void visitIfStmt(IfStmt *node) {}

    virtual void visitWhileStmt(WhileStmt *node) {}

    virtual void visitForStmt(ForStmt *node) {}

    virtual void visitForEachStmt(ForEachStmt *node) {}

    virtual void visitMatchStmt(MatchStmt *node) {}

    virtual void visitBlock(Block *node) {}

    virtual void visitFuncDef(FuncDef *node) {}

    virtual void visitCompilationUnit(CompilationUnit *node) {}
};


//===----------------------------------------------------------------------===//
// Top level ast nodes
//===----------------------------------------------------------------------===//
struct AstNode {
    explicit AstNode(int line, int column) : line(line), column(column) {}

    virtual ~AstNode() = default;

    int line = -1;
    int column = -1;

    virtual void visit(AstVisitor *visitor) {}

    virtual const char *name() const { return "AstNode"; }
};

struct Expr : public AstNode {
    using AstNode::AstNode;

    virtual ~Expr() = default;

    void visit(AstVisitor *visitor) override { visitor->visitExpr(this); }

    const char *name() const override { return "Expr"; }
};

struct Stmt : public AstNode {
    using AstNode::AstNode;

    virtual ~Stmt() = default;

    void visit(AstVisitor *visitor) override { visitor->visitStmt(this); }

    const char *name() const override { return "Stmt"; }
};

struct Block : public AstNode {
    using AstNode::AstNode;

    std::vector<Stmt *> stmts;

    void visit(AstVisitor *visitor) override { visitor->visitBlock(this); }

    const char *name() const override { return "Block"; }
};

struct FuncDef : public AstNode {
    using AstNode::AstNode;

    std::string funcName;
    std::vector<std::string> params;
    Block *block{};

    void visit(AstVisitor *visitor) override { visitor->visitFuncDef(this); }

    const char *name() const override { return "FuncDef"; }
};

struct CompilationUnit : public AstNode {
    explicit CompilationUnit() : AstNode(-1, -1) {}

    std::vector<FuncDef *> definitions;
    std::vector<Stmt *> topStmts;

    void visit(AstVisitor *visitor) override { visitor->visitCompilationUnit(this); }

    const char *name() const override { return "CompilationUnit"; }
};

//===----------------------------------------------------------------------===//
// Expressions
//===----------------------------------------------------------------------===//

#define AST_EXPR(expr_name) \
struct expr_name : public Expr{ \
    using Expr::Expr;\
    void visit(AstVisitor *visitor) override { visitor->visit##expr_name(this); } \
    const char * name() const override { return ""#expr_name;}

#define AST_EXPR_END };

AST_EXPR(BoolExpr)

    bool literal;
AST_EXPR_END

AST_EXPR(CharExpr)

    char literal;
AST_EXPR_END

AST_EXPR(NullExpr)
AST_EXPR_END

AST_EXPR(IntExpr)

    int literal;
AST_EXPR_END

AST_EXPR(DoubleExpr)

    double literal;
AST_EXPR_END

AST_EXPR(StringExpr)

    std::string literal;
AST_EXPR_END

AST_EXPR(ArrayExpr)

    std::vector<Expr *> literal;
AST_EXPR_END

AST_EXPR(IdentExpr)

    std::string identName;
AST_EXPR_END

AST_EXPR(IndexExpr)

    std::string identName;
    Expr *index{};
AST_EXPR_END

AST_EXPR(BinaryExpr)

    Expr *lhs{};
    Token opt{};
    Expr *rhs{};
AST_EXPR_END

AST_EXPR(FuncCallExpr)

    std::string funcName;
    std::vector<Expr *> args;
AST_EXPR_END

AST_EXPR(AssignExpr)

    Expr *lhs{};
    Token opt;
    Expr *rhs{};
AST_EXPR_END

AST_EXPR(ClosureExpr)

    std::vector<std::string> params;
    Block *block{};
AST_EXPR_END

//===----------------------------------------------------------------------===//
// Stmt
//===----------------------------------------------------------------------===//

#define AST_STMT(stmt_name) \
struct stmt_name : public Stmt{ \
    using Stmt::Stmt;\
    void visit(AstVisitor *visitor) override { visitor->visit##stmt_name(this); } \
    const char * name() const override { return ""#stmt_name;}

#define AST_STMT_END };

AST_STMT(BreakStmt)
AST_STMT_END

AST_STMT(ContinueStmt)
AST_STMT_END

AST_STMT(SimpleStmt)

    Expr *expr{};
AST_STMT_END

AST_STMT(ReturnStmt)

    Expr *retval{};
AST_STMT_END

AST_STMT(IfStmt)

    Expr *cond{};
    Block *block{};
    Block *elseBlock{};
AST_STMT_END

AST_STMT(WhileStmt)

    Expr *cond{};
    Block *block{};
AST_STMT_END

AST_STMT(ForStmt)

    Expr *init{};
    Expr *cond{};
    Expr *post{};
    Block *block{};
AST_STMT_END

AST_STMT(ForEachStmt)

    std::string identName;
    Expr *list{};
    Block *block{};
AST_STMT_END

AST_STMT(MatchStmt)

    Expr *cond{};
    std::vector<std::tuple<Expr *, Block *, bool>> matches;
AST_STMT_END

#endif