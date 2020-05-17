#include "BytecodeGenerator.h"
#include "Opcode.h"

class Label;

class Jmp {
private:
    int patching;
public:
    explicit Jmp(BytecodeGenerator *gen, Label *label, Opcode opcode);

    [[nodiscard]] int getPatching() const { return patching; }
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
    for (auto *definition:node->definitions) {
        definition->visit(this);
    }
    for (auto *stmt:node->topStmts) {
        stmt->visit(this);
    }
}

void BytecodeGenerator::visitBoolExpr(BoolExpr *node) {
    constInt(node->literal);
}

void BytecodeGenerator::visitCharExpr(CharExpr *node) {
    constInt(node->literal);
}

void BytecodeGenerator::visitNullExpr(NullExpr *node) {
    bytecode->bytecodes[bci++] = CONST_NULL;
}

void BytecodeGenerator::visitIntExpr(IntExpr *node) {
    constInt(node->literal);
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
    bytecode->bytecodes[bci++] = NEW_ARR;
    bytecode->bytecodes[bci++] = node->literal.size();
    for (int i = 0; i < node->literal.size(); i++) {
        bytecode->bytecodes[bci++] = DUP;
        node->literal[i]->visit(this);
        constInt(i);
        bytecode->bytecodes[bci++] = STORE_INDEX;
    }
}

void BytecodeGenerator::visitIdentExpr(IdentExpr *node) {
    if (auto iter = localMap.find(node->identName);iter == localMap.cend()) {
        panic("variable undefined but using");
    }
    int localIndex = localMap[node->identName];
    varLoad(localIndex);
}

void BytecodeGenerator::visitIndexExpr(IndexExpr *node) {
    if (auto iter = localMap.find(node->identName);iter == localMap.cend()) {
        panic("variable undefined but using");
    }
    int localIndex = localMap[node->identName];
    varLoad(localIndex);
    node->index->visit(this);
    bytecode->bytecodes[bci++] = LOAD_INDEX;
}

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
    if (typeid(*node->lhs) == typeid(IndexExpr)) {
        // Array element
        auto *t = dynamic_cast<IndexExpr *>(node->lhs);
        int localIndex = localMap[t->identName];
        varLoad(localIndex);
        node->rhs->visit(this);
        t->index->visit(this);
        bytecode->bytecodes[bci++] = STORE_INDEX;
    } else if (typeid(*node->lhs) == typeid(IdentExpr)) {
        // Normal variable
        node->rhs->visit(this);
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
        node->retval->visit(this);
        bytecode->bytecodes[bci++] = RETURN_VAL;
    }

}

void BytecodeGenerator::visitIfStmt(IfStmt *node) {
    if (typeid(*(node->cond)) == typeid(BinaryExpr)) {
        auto *shortCircuit = dynamic_cast<BinaryExpr *>(node->cond);
        if (shortCircuit->opt == TK_LOGAND) {
            if (node->elseBlock == nullptr) {
                Label L_out(this);

                shortCircuit->lhs->visit(this);
                Jmp j1(this, &L_out, JMP_NE);
                shortCircuit->rhs->visit(this);
                Jmp j2(this, &L_out, JMP_NE);
                node->block->visit(this);
                L_out();
            } else {
                Label L_out(this);
                Label L_else(this);

                shortCircuit->lhs->visit(this);
                Jmp j1(this, &L_else, JMP_NE);
                shortCircuit->rhs->visit(this);
                Jmp j2(this, &L_else, JMP_NE);
                node->block->visit(this);
                Jmp j3(this, &L_out, JMP);
                L_else();
                node->elseBlock->visit(this);
                L_out();
            }
            return;
        } else if (shortCircuit->opt == TK_LOGOR) {
            if (node->elseBlock == nullptr) {
                Label L_then(this);
                Label L_out(this);

                shortCircuit->lhs->visit(this);
                Jmp j1(this, &L_then, JMP_EQ);
                shortCircuit->rhs->visit(this);
                Jmp j2(this, &L_then, JMP_EQ);
                Jmp j3(this, &L_out, JMP);
                L_then();
                node->block->visit(this);
                L_out();
            } else {
                Label L_out(this);
                Label L_else(this);
                Label L_then(this);

                shortCircuit->lhs->visit(this);
                Jmp j1(this, &L_then, JMP_EQ);
                shortCircuit->rhs->visit(this);
                Jmp j2(this, &L_then, JMP_EQ);
                Jmp j3(this, &L_else, JMP);
                L_then();
                node->block->visit(this);
                Jmp j4(this, &L_out, JMP);
                L_else();
                node->elseBlock->visit(this);
                L_out();
            }
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

void BytecodeGenerator::constInt(nyx::int32 integer) {
    bytecode->bytecodes[bci++] = CONST_I;
    *(nyx::int32 *) (bytecode->bytecodes + bci) = integer;
    bci += 4;
}

void BytecodeGenerator::varLoad(int localIndex) {
    bytecode->bytecodes[bci++] = LOAD;
    bytecode->bytecodes[bci++] = localIndex;
}


BytecodeGenerator::BytecodeGenerator() {
    this->bytecode = new Bytecode;
    this->bci = 0;
    this->local = 0;
}

void BytecodeGenerator::fixupBytecode() {
    bytecode->bytecodeSize = bci;
    bytecode->localSize = local;
}

Bytecode *BytecodeGenerator::generate(CompilationUnit *unit) {
    {
        // Generate bytecode via visitor pattern
        PhaseTime timer("generate bytecode from Ast");
        unit->visit(this);
    }
    fixupBytecode();
    return bytecode;
}

Bytecode *BytecodeGenerator::generate(FuncDef *node) {
    // Prepare parameters
    for (int i = 0; i < node->params.size(); i++) {
        localMap.insert({node->params[i], i});
    }
    // Generate bytecode via visitor pattern
    node->block->visit(this);
    // Fixup generated production
    fixupBytecode();
    // don't delete node, it's responsibility of public generate() API
    return bytecode;
}
