#include "BytecodeGenerator.h"
#include "Opcode.h"

class Label;

class Jmp {
private:
    int patching;
public:
    explicit Jmp(BytecodeGenerator *gen, Label *label, Opcode opcode);

    int getPatching() const { return patching; }
};

class Label {
private:
    int destination;
    BytecodeGenerator *gen;
    std::vector<Jmp *> allJump;
public:
    explicit Label(BytecodeGenerator *gen) {
        this->gen = gen;
        this->destination = gen->bci;
    }

    void operator()() {
        // reset destination
        this->destination = gen->bci;
    }

    void addJump(Jmp *jmp) { allJump.push_back(jmp); }

    ~Label();
};

Label::~Label() {
    for (auto &i : allJump) {
        gen->bytecode->bytecodes[i->getPatching()] = destination;
    }
}

Jmp::Jmp(BytecodeGenerator *gen, Label *label, Opcode opcode) {
    // Generate JMP* bytecode
    gen->bytecode->bytecodes[gen->bci++] = opcode;
    // Record JMP* target as patching address and temporarily set to -1
    this->patching = gen->bci;
    gen->bytecode->bytecodes[gen->bci++] = -1;
    label->addJump(this);
}


void BytecodeGenerator::visitBlock(Block *node) {
    for (auto *stmt:node->stmts) {
        stmt->visit(this);
    }
}

void BytecodeGenerator::visitFuncDef(FuncDef *node) {
    // Create new BytecodeGenerator and produce bytecodes
    BytecodeGenerator gen;
    auto *funcBytecode = gen.generate(node);
    bytecode->functions.insert(std::make_pair(node->funcName, funcBytecode));
}

void BytecodeGenerator::visitCompilationUnit(CompilationUnit *node) {
    for (auto *stmt:node->topStmts) {
        stmt->visit(this);
    }
}

void BytecodeGenerator::visitBoolExpr(BoolExpr *node) {
    bytecode->bytecodes[bci++] = CONST_I;
    *(nyx::int32 *) (bytecode->bytecodes + bci) = node->literal;
    bci += 4;
}

void BytecodeGenerator::visitCharExpr(CharExpr *node) {
    bytecode->bytecodes[bci++] = CONST_I;
    *(nyx::int32 *) (bytecode->bytecodes + bci) = node->literal;
    bci += 4;
}

void BytecodeGenerator::visitNullExpr(NullExpr *node) {
    bytecode->bytecodes[bci++] = CONST_NULL;
}

void BytecodeGenerator::visitIntExpr(IntExpr *node) {
    bytecode->bytecodes[bci++] = CONST_I;
    *(nyx::int32 *) (bytecode->bytecodes + bci) = node->literal;
    bci += 4;
}

void BytecodeGenerator::visitExpr(Expr *expr) {
    panic("should not reach here");
}

void BytecodeGenerator::visitDoubleExpr(DoubleExpr *node) {
    bytecode->bytecodes[bci++] = CONST_D;
    *(double *) (bytecode->bytecodes + bci) = node->literal;
    bci += 8;
}

void BytecodeGenerator::visitStringExpr(StringExpr *node) {
    bytecode->strings.push_back(node->literal);
    bytecode->bytecodes[bci++] = CONST_STR;
    bytecode->bytecodes[bci++] = bytecode->strings.size() - 1;
}

void BytecodeGenerator::visitArrayExpr(ArrayExpr *node) {
}

void BytecodeGenerator::visitIdentExpr(IdentExpr *node) {
    if (auto iter = localMap.find(node->identName);iter == localMap.cend()) {
        panic("variable undefined but using");
    }
    int localIndex = localMap[node->identName];
    bytecode->bytecodes[bci++] = LOAD;
    bytecode->bytecodes[bci++] = localIndex;
}

void BytecodeGenerator::visitIndexExpr(IndexExpr *node) {}

void BytecodeGenerator::visitBinaryExpr(BinaryExpr *node) {
    if (node->rhs == nullptr) {
        // unary expression
        node->lhs->visit(this);
        switch (node->opt) {
            case TK_MINUS:
                bytecode->bytecodes[bci++] = NEG;
                break;
            case TK_LOGNOT:
                bytecode->bytecodes[bci++] = CONST_I;
                *(nyx::int32 *) (bytecode->bytecodes + bci) = 1;
                bci += 4;
                bytecode->bytecodes[bci++] = TEST_NE;
                break;
            case TK_BITNOT:
                bytecode->bytecodes[bci++] = NOT;
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
                bytecode->bytecodes[bci++] = OR;
                break;
            case TK_BITAND:
                node->lhs->visit(this);
                node->rhs->visit(this);
                bytecode->bytecodes[bci++] = AND;
                break;
            case TK_LOGOR:
            case TK_LOGAND:
                panic("should be specially handled in control flow statements");
            case TK_EQ:
                node->lhs->visit(this);
                node->rhs->visit(this);
                bytecode->bytecodes[bci++] = TEST_EQ;
                break;
            case TK_NE:
                node->lhs->visit(this);
                node->rhs->visit(this);
                bytecode->bytecodes[bci++] = TEST_NE;
                break;
            case TK_GT:
                node->lhs->visit(this);
                node->rhs->visit(this);
                bytecode->bytecodes[bci++] = TEST_GT;
                break;
            case TK_GE:
                node->lhs->visit(this);
                node->rhs->visit(this);
                bytecode->bytecodes[bci++] = TEST_GE;
                break;
            case TK_LT:
                node->lhs->visit(this);
                node->rhs->visit(this);
                bytecode->bytecodes[bci++] = TEST_LT;
                break;
            case TK_LE:
                node->lhs->visit(this);
                node->rhs->visit(this);
                bytecode->bytecodes[bci++] = TEST_LE;
                break;
            case TK_PLUS:
                node->lhs->visit(this);
                node->rhs->visit(this);
                bytecode->bytecodes[bci++] = ADD;
                break;
            case TK_MINUS:
                node->lhs->visit(this);
                node->rhs->visit(this);
                bytecode->bytecodes[bci++] = SUB;
                break;
            case TK_MOD:
                node->lhs->visit(this);
                node->rhs->visit(this);
                bytecode->bytecodes[bci++] = REM;
                break;
            case TK_TIMES:
                node->lhs->visit(this);
                node->rhs->visit(this);
                bytecode->bytecodes[bci++] = MUL;
                break;
            case TK_DIV:
                node->lhs->visit(this);
                node->rhs->visit(this);
                bytecode->bytecodes[bci++] = DIV;
                break;
            default:
                panic("should not reach here");
        }
    }

}

void BytecodeGenerator::visitFuncCallExpr(FuncCallExpr *node) {
    for (auto *arg:node->args) {
        arg->visit(this);
    }
    bytecode->bytecodes[bci++] = CALL;
    bytecode->strings.push_back(node->funcName);
    bytecode->bytecodes[bci++] = bytecode->strings.size() - 1;
    bytecode->bytecodes[bci++] = node->args.size();
}

void BytecodeGenerator::visitAssignExpr(AssignExpr *node) {
    node->rhs->visit(this);
    if (typeid(*node->lhs) == typeid(IndexExpr)) {
        // Array element
        panic("shout not reach here");
    } else if (typeid(*node->lhs) == typeid(IdentExpr)) {
        // Normal variable
        auto *t = dynamic_cast<IdentExpr *>(node->lhs);
        bytecode->bytecodes[bci++] = STORE;
        int localIndex = local++;
        localMap.insert(std::make_pair(t->identName, localIndex));
        bytecode->bytecodes[bci++] = localIndex;
    }
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
        bytecode->bytecodes[bci++] = RETURN;
    } else {
        // TODO
    }

}

void BytecodeGenerator::visitIfStmt(IfStmt *node) {
    if (typeid(*(node->cond)) == typeid(BinaryExpr)) {
        auto *shortCircuit = dynamic_cast<BinaryExpr *>(node->cond);
        if (shortCircuit->opt == TK_LOGAND) {
/**
 * &&
 * cond1
 * jmp_ne out
 * cond2
 * jmp_ne out
 * <then>
 * jmp
 */
            return;
        } else if (shortCircuit->opt == TK_LOGOR) {
            return;
        }
    }
    node->cond->visit(this);
    if (node->elseBlock == nullptr) {
        Label L_out(this);

        Jmp j1(this, &L_out, JMP_NE);
        node->block->visit(this);
        L_out();
    } else {
        Label L_else(this);
        Label L_out(this);

        Jmp j1(this, &L_else, JMP_NE);
        node->block->visit(this);
        Jmp j2(this, &L_out, JMP);
        L_else();
        node->elseBlock->visit(this);
        Jmp j3(this, &L_out, JMP);
        L_out();
    }
}

void BytecodeGenerator::visitWhileStmt(WhileStmt *node) {}

void BytecodeGenerator::visitForStmt(ForStmt *node) {}

void BytecodeGenerator::visitForEachStmt(ForEachStmt *node) {}

void BytecodeGenerator::visitMatchStmt(MatchStmt *node) {}

void BytecodeGenerator::visitImportStmt(ImportStmt *node) {}

void BytecodeGenerator::visitExportStmt(ExportStmt *node) {}

Bytecode *BytecodeGenerator::generate(CompilationUnit *unit) {
    // Setup related data structures
    this->bytecode = new Bytecode;
    this->bci = 0;
    this->local = 0;
    {
        // Generate bytecode via visitor pattern
        PhaseTime timer("generate bytecode from Ast");
        unit->visit(this);
    }
    // Fixup generated production
    bytecode->bytecodeSize = bci;
    bytecode->localSize = local;
    delete unit;
    return bytecode;
}

Bytecode *BytecodeGenerator::generate(FuncDef *node) {
    // Setup related data structures
    this->bytecode = new Bytecode;
    this->bci = 0;
    this->local = 0;
    // Generate bytecode via visitor pattern
    node->visit(this);
    // Fixup generated production
    bytecode->bytecodeSize = bci;
    bytecode->localSize = local;
    // don't delete node, it's responsibility of public generate() API
    return bytecode;
}
