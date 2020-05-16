#include "BytecodeGenerator.h"
#include "Opcode.h"

void BytecodeGenerator::visitBlock(Block *node) {
    for (auto *stmt:node->stmts) {
        stmt->visit(this);
    }
}

void BytecodeGenerator::visitFuncDef(FuncDef *node) {}

void BytecodeGenerator::visitCompilationUnit(CompilationUnit *node) {
    for (auto *stmt:node->topStmts) {
        stmt->visit(this);
    }
}

void BytecodeGenerator::visitBoolExpr(BoolExpr *node) {
    meta->bytecodes[bci++] = CONST_I;
    meta->bytecodes[bci++] = (int) node->literal;
}

void BytecodeGenerator::visitCharExpr(CharExpr *node) {
    meta->bytecodes[bci++] = CONST_I;
    meta->bytecodes[bci++] = (int) node->literal;
}

void BytecodeGenerator::visitNullExpr(NullExpr *node) {
    meta->bytecodes[bci++] = CONST_NULL;
}

void BytecodeGenerator::visitIntExpr(IntExpr *node) {
    meta->bytecodes[bci++] = CONST_I;
    *(nyx::int32 *) (meta->bytecodes + bci) = node->literal;
    bci += 4;
}

void BytecodeGenerator::visitExpr(Expr *expr) {
    panic("should not reach here");
}

void BytecodeGenerator::visitDoubleExpr(DoubleExpr *node) {
    meta->bytecodes[bci++] = CONST_D;
    for (int i = 0; i < 8; i++) {
        meta->bytecodes[bci++] = (reinterpret_cast<nyx::int8 *>(&(node->literal)))[i];
    }

}

void BytecodeGenerator::visitStringExpr(StringExpr *node) {
    meta->strings.push_back(node->literal);
    meta->bytecodes[bci++] = CONST_STR;
    meta->bytecodes[bci++] = meta->strings.size() - 1;
}

void BytecodeGenerator::visitArrayExpr(ArrayExpr *node) {
}

void BytecodeGenerator::visitIdentExpr(IdentExpr *node) {}

void BytecodeGenerator::visitIndexExpr(IndexExpr *node) {}

void BytecodeGenerator::visitBinaryExpr(BinaryExpr *node) {
    if (node->rhs == nullptr) {
        // unary expression
        node->lhs->visit(this);
        switch (node->opt) {
            case TK_MINUS:
                meta->bytecodes[bci++] = NEG;
                break;
            case TK_LOGNOT:
                meta->bytecodes[bci++] = TEST_NE;
                break;
            case TK_BITNOT:
                meta->bytecodes[bci++] = NOT;
                break;
            default:
                panic("should not reach here");
        }
    } else {
        // binary expression
        switch (node->opt) {
            case TK_BITOR:
                node->lhs->visit(this);
                node->rhs->visit(this);
                meta->bytecodes[bci++] = OR;
                break;
            case TK_BITAND:
                node->lhs->visit(this);
                node->rhs->visit(this);
                meta->bytecodes[bci++] = AND;
                break;
            case TK_LOGOR:
                //todo
                break;
            case TK_LOGAND:
                //todo
                break;
            case TK_EQ:
                node->lhs->visit(this);
                node->rhs->visit(this);
                meta->bytecodes[bci++] = TEST_EQ;
                break;
            case TK_NE:
                node->lhs->visit(this);
                node->rhs->visit(this);
                meta->bytecodes[bci++] = TEST_NE;
                break;
            case TK_GT:
                node->lhs->visit(this);
                node->rhs->visit(this);
                meta->bytecodes[bci++] = TEST_GT;
                break;
            case TK_GE:
                node->lhs->visit(this);
                node->rhs->visit(this);
                meta->bytecodes[bci++] = TEST_GE;
                break;
            case TK_LT:
                node->lhs->visit(this);
                node->rhs->visit(this);
                meta->bytecodes[bci++] = TEST_LT;
                break;
            case TK_LE:
                node->lhs->visit(this);
                node->rhs->visit(this);
                meta->bytecodes[bci++] = TEST_EQ;
                break;
            case TK_PLUS:
                node->lhs->visit(this);
                node->rhs->visit(this);
                meta->bytecodes[bci++] = ADD;
                break;
            case TK_MINUS:
                node->lhs->visit(this);
                node->rhs->visit(this);
                meta->bytecodes[bci++] = SUB;
                break;
            case TK_MOD:
                node->lhs->visit(this);
                node->rhs->visit(this);
                meta->bytecodes[bci++] = REM;
                break;
            case TK_TIMES:
                node->lhs->visit(this);
                node->rhs->visit(this);
                meta->bytecodes[bci++] = MUL;
                break;
            case TK_DIV:
                node->lhs->visit(this);
                node->rhs->visit(this);
                meta->bytecodes[bci++] = DIV;
                break;
            default:
                panic("should not reach here");
        }
    }

}


//===----------------------------------------------------------------------===//
// call the function either in this module or in one of imported modules
//
// CALL FUNC_NAME ARG_NUM
// stack_bottom -> stack_top
// before: (arg_n, arg_n-1,... arg2, arg1)
// after: ()
//===----------------------------------------------------------------------===//
void BytecodeGenerator::visitFuncCallExpr(FuncCallExpr *node) {
    for (auto *arg:node->args) {
        arg->visit(this);
    }
    meta->bytecodes[bci++] = CALL;
    meta->strings.push_back(node->funcName);
    meta->bytecodes[bci++] = meta->strings.size() - 1;
    meta->bytecodes[bci++] = node->args.size();
}

void BytecodeGenerator::visitAssignExpr(AssignExpr *node) {

}

void BytecodeGenerator::visitClosureExpr(ClosureExpr *node) {}

void BytecodeGenerator::visitStmt(Stmt *node) {
    panic("should not reach here");
}

void BytecodeGenerator::visitBreakStmt(BreakStmt *node) {}

void BytecodeGenerator::visitContinueStmt(ContinueStmt *node) {}

void BytecodeGenerator::visitSimpleStmt(SimpleStmt *node) {
    node->expr->visit(this);
}

void BytecodeGenerator::visitReturnStmt(ReturnStmt *node) {
    if (node->retval == nullptr) {
        meta->bytecodes[bci++] = RETURN;
    } else {
        node->retval->visit(this);
    }

}

void BytecodeGenerator::visitIfStmt(IfStmt *node) {
    node->cond->visit(this);
    if (node->elseBlock == nullptr) {
        meta->bytecodes[bci++] = JMP_NE;
        int patching = bci;
        meta->bytecodes[bci++] = -1; // target, further patching
        node->block->visit(this);
        meta->bytecodes[patching] = bci;
    } else {
        meta->bytecodes[bci++] = JMP_NE;
        int elsePatching = bci;
        meta->bytecodes[bci++] = -1;
        node->block->visit(this);
        meta->bytecodes[bci++] = JMP;
        int normalPatching = bci;
        meta->bytecodes[bci++] = -1;
        meta->bytecodes[elsePatching] = bci;
        node->elseBlock->visit(this);
        meta->bytecodes[bci++] = JMP;
        meta->bytecodes[bci++] = bci + 1;
        meta->bytecodes[normalPatching] = bci;
    }
}

void BytecodeGenerator::visitWhileStmt(WhileStmt *node) {}

void BytecodeGenerator::visitForStmt(ForStmt *node) {}

void BytecodeGenerator::visitForEachStmt(ForEachStmt *node) {}

void BytecodeGenerator::visitMatchStmt(MatchStmt *node) {}

void BytecodeGenerator::visitImportStmt(ImportStmt *node) {}

void BytecodeGenerator::visitExportStmt(ExportStmt *node) {}

MetaArea *BytecodeGenerator::generate() {
    PhaseTime timer("generate bytecode from Ast");
    unit->visit(this);
    meta->bytecodeSize = bci;
    return meta;
}

BytecodeGenerator::BytecodeGenerator(CompilationUnit *unit) : unit(unit) {
    meta = new MetaArea;
    meta->bytecodes = new nyx::int32[65535];
    meta->bytecodeSize = -1;
    bci = 0;
}

