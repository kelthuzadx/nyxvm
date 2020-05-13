#ifndef NYX_ASTVISITOR_H
#define NYX_ASTVISITOR_H


struct AstVisitor {
    virtual void visitBoolExpr(BoolExpr *node) {

    }

    virtual void visitCharExpr(CharExpr *node) {

    }

    virtual void visitNullExpr(NullExpr *node) {

    }

    virtual void visitIntExpr(IntExpr *node) {

    }

    virtual void visitExpr(Expr *expr) {

    }

    virtual void visitDoubleExpr(DoubleExpr *node) {

    }

    virtual void visitStringExpr(StringExpr *node) {

    }

    virtual void visitArrayExpr(ArrayExpr *node) {

    }

    virtual void visitIdentExpr(IdentExpr *node) {

    }

    virtual void visitIndexExpr(IndexExpr *node) {

    }

    virtual void visitBinaryExpr(BinaryExpr *node) {

    }

    virtual void visitFuncCallExpr(FuncCallExpr *node) {

    }

    virtual void visitAssignExpr(AssignExpr *node) {

    }

    virtual void visitClosureExpr(ClosureExpr *node) {

    }

    virtual void visitStmt(Stmt *node) {

    }

    virtual void visitBreakStmt(BreakStmt *node) {

    }

    virtual void visitContinueStmt(ContinueStmt *node) {

    }

    virtual void visitSimpleStmt(SimpleStmt *node) {

    }

    virtual void visitReturnStmt(ReturnStmt *node) {

    }

    virtual void visitIfStmt(IfStmt *node) {

    }

    virtual void visitWhileStmt(WhileStmt *node) {

    }

    virtual void visitForStmt(ForStmt *node) {

    }

    virtual void visitForEachStmt(ForEachStmt *node) {

    }

    virtual void visitMatchStmt(MatchStmt *node) {

    }

    virtual void visitBlock(Block *node) {

    }

    virtual void visitFuncDef(FuncDef *node) {

    }

    virtual void visitCompilationUnit(CompilationUnit *node) {

    }
};


#endif //NYX_ASTVISITOR_H
