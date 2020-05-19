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
    std::vector<Jmp> allJump;
public:
    explicit Label(BytecodeGenerator *gen);

    void operator()();

    void addJump(Jmp jmp) { allJump.push_back(jmp); }

    void setGenerator(BytecodeGenerator *gen);

    ~Label();

    Label();
};

Label::~Label() {
    for (auto &i : allJump) {
        gen->bytecode->bytecodes[i.getPatching()] = destination;
    }
}

Label::Label(BytecodeGenerator *gen) {
    this->gen = gen;
    this->destination = gen->bci;
}

void Label::operator()() {
    // reset destination
    this->destination = gen->bci;
}

Label::Label() {
    this->gen = nullptr;
    this->destination = -1;
}

void Label::setGenerator(BytecodeGenerator *gen) {
    this->gen = gen;
    this->destination = gen->bci;
}

Jmp::Jmp(BytecodeGenerator *gen, Label *label, Opcode opcode) {
    // Generate JMP* bytecode
    gen->bytecode->bytecodes[gen->bci++] = opcode;
    // Record JMP* target as patching address and temporarily set to -1
    this->patching = gen->bci;
    gen->bytecode->bytecodes[gen->bci++] = -1;
    label->addJump(*this);
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
    constStr(node->literal);
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
                constInt(1);
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
    if (!node->funcName.empty()) {
        // normal function calling
        constStr(node->funcName);
        for (auto *arg:node->args) {
            arg->visit(this);
        }
        bytecode->bytecodes[bci++] = CALL;
        bytecode->bytecodes[bci++] = node->args.size();
    } else {
        // closure calling
    }

}

void BytecodeGenerator::visitAssignExpr(AssignExpr *node) {
    // arr[index] = value
    if (typeid(*node->lhs) == typeid(IndexExpr)) {
        auto *t = dynamic_cast<IndexExpr *>(node->lhs);
        if (node->opt == TK_ASSIGN) {
            varLoad(localMap[t->identName]);
            node->rhs->visit(this);
            t->index->visit(this);
            bytecode->bytecodes[bci++] = STORE_INDEX;
        } else {
            varLoad(localMap[t->identName]);
            varLoad(localMap[t->identName]);
            t->index->visit(this);
            bytecode->bytecodes[bci++] = LOAD_INDEX;
            node->rhs->visit(this);
            switch (node->opt) {
                case TK_PLUS_AGN:
                    bytecode->bytecodes[bci++] = ADD;
                    break;
                case TK_MINUS_AGN:
                    bytecode->bytecodes[bci++] = SUB;
                    break;
                case TK_TIMES_AGN:
                    bytecode->bytecodes[bci++] = MUL;
                    break;
                case TK_DIV_AGN:
                    bytecode->bytecodes[bci++] = DIV;
                    break;
                case TK_MOD_AGN:
                    bytecode->bytecodes[bci++] = REM;
                    break;
                default:
                    panic("should not reach here");
            }
            t->index->visit(this);
            bytecode->bytecodes[bci++] = STORE_INDEX;
        }

    }
        // var = value
    else if (typeid(*node->lhs) == typeid(IdentExpr)) {
        auto *t = dynamic_cast<IdentExpr *>(node->lhs);
        if (node->opt == TK_ASSIGN) {
            node->rhs->visit(this);
            if (auto iter = localMap.find(t->identName);iter != localMap.cend()) {
                // reassign existing variable a new value
                varStore(localMap[t->identName]);
            } else {
                // create a new variable
                varNew(t->identName);
            }

        } else {
            varLoad(localMap[t->identName]);
            node->rhs->visit(this);
            switch (node->opt) {
                case TK_PLUS_AGN:
                    bytecode->bytecodes[bci++] = ADD;
                    break;
                case TK_MINUS_AGN:
                    bytecode->bytecodes[bci++] = SUB;
                    break;
                case TK_TIMES_AGN:
                    bytecode->bytecodes[bci++] = MUL;
                    break;
                case TK_DIV_AGN:
                    bytecode->bytecodes[bci++] = DIV;
                    break;
                case TK_MOD_AGN:
                    bytecode->bytecodes[bci++] = REM;
                    break;
                default:
                    panic("should not reach here");
            }
            varStore(localMap[t->identName]);
        }
    }
}

void BytecodeGenerator::visitClosureExpr(ClosureExpr *node) {
    //TODO

}

void BytecodeGenerator::visitStmt(Stmt *node) {
    panic("should not reach here");
}

void BytecodeGenerator::visitBreakStmt(BreakStmt *node) {
    Jmp j1(this, this->breakPoint, JMP);
}

void BytecodeGenerator::visitContinueStmt(ContinueStmt *node) {
    Jmp j1(this, this->continuePoint, JMP);
}

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
    if (isShortCircuitAnd(node->cond)) {
        auto *shortCircuit = dynamic_cast<BinaryExpr *>(node->cond);
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
    }
    if (isShortCircuitOr(node->cond)) {
        auto *shortCircuit = dynamic_cast<BinaryExpr *>(node->cond);
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


    if (node->elseBlock == nullptr) {
        /**
         * <cond>
         * jmp_ne out
         * <block>
         * out:
         */
        Label L_out(this);

        node->cond->visit(this);
        Jmp j1(this, &L_out, JMP_NE);
        node->block->visit(this);
        L_out();
    } else {
        /**
         * <cond>
         * jmp_ne else
         * <block>
         * jmp out
         * else:
         * <else-block>
         * jmp out
         * out:
         */
        Label L_else(this);
        Label L_out(this);

        node->cond->visit(this);
        Jmp j1(this, &L_else, JMP_NE);
        node->block->visit(this);
        Jmp j2(this, &L_out, JMP);
        L_else();
        node->elseBlock->visit(this);
        Jmp j3(this, &L_out, JMP);
        L_out();
    }
}

void BytecodeGenerator::visitWhileStmt(WhileStmt *node) {
    if (isShortCircuitAnd(node->cond)) {
        /**
         * cond:
         * <cond_lhs>
         * jmp_ne out
         * <cond_rhs>
         * jmp_ne out
         * <block>
         * jmp cond
         * out:
         */
        auto *t = dynamic_cast<BinaryExpr *>(node->cond);
        Label L_cond(this);
        Label L_out(this);

        t->lhs->visit(this);
        Jmp j1(this, &L_out, JMP_NE);
        t->rhs->visit(this);
        Jmp j2(this, &L_out, JMP_NE);
        node->block->visit(this);
        Jmp j3(this, &L_cond, JMP);
        L_out();
        return;
    }
    if (isShortCircuitOr(node->cond)) {
        /**
         * cond:
         * <cond_lhs>
         * jmp_eq then
         * <cond_rhs>
         * jmp_ne then
         * jmp out
         * then:
         * <block>
         * jmp cond
         * out:
         */
        auto *t = dynamic_cast<BinaryExpr *>(node->cond);
        Label L_cond(this);
        Label L_out(this);
        Label L_then(this);

        t->lhs->visit(this);
        Jmp j1(this, &L_then, JMP_EQ);
        t->rhs->visit(this);
        Jmp j2(this, &L_then, JMP_EQ);
        Jmp j3(this, &L_out, JMP);
        L_then();
        node->block->visit(this);
        Jmp j4(this, &L_cond, JMP);
        L_out();
        return;
    }

    /**
     * cond:
     * <cond>
     * jmp_neq out
     * <block>
     * jmp cond
     * out:
     */
    Label L_cond(this);
    Label L_out(this);

    node->cond->visit(this);
    Jmp j1(this, &L_out, JMP_NE);
    node->block->visit(this);
    Jmp j2(this, &L_cond, JMP);
    L_out();
}

void BytecodeGenerator::visitForStmt(ForStmt *node) {
    if (isShortCircuitAnd(node->cond)) {
        /**
         * <init>
         * cond:
         * <cond_lhs>
         * jmp_ne out
         * <cond_rhs>
         * jmp_ne out
         * <block>
         * continue:
         * <post>
         * jmp cond
         * out:
         */
        auto *t = dynamic_cast<BinaryExpr *>(node->cond);
        node->init->visit(this);

        Label L_cond(this);
        Label L_out(this);
        Label L_continue(this);
        this->continuePoint = &L_continue;
        this->breakPoint = &L_out;

        t->lhs->visit(this);
        Jmp j1(this, &L_out, JMP_NE);
        t->rhs->visit(this);
        Jmp j2(this, &L_out, JMP_NE);
        node->block->visit(this);
        L_continue();
        node->post->visit(this);
        Jmp j3(this, &L_cond, JMP);
        L_out();

        this->continuePoint = nullptr;
        this->breakPoint = nullptr;
        return;
    }
    if (isShortCircuitOr(node->cond)) {
        /**
         * <init>
         * cond:
         * <cond_lhs>
         * jmp_eq then
         * <cond_rhs>
         * jmp_eq then
         * jmp out
         * then:
         * <block>
         * continue:
         * <post>
         * jmp cond
         * out:
         */
        auto *t = dynamic_cast<BinaryExpr *>(node->cond);
        node->init->visit(this);

        Label L_cond(this);
        Label L_then(this);
        Label L_out(this);
        Label L_continue(this);
        this->continuePoint = &L_continue;
        this->breakPoint = &L_out;

        t->lhs->visit(this);
        Jmp j1(this, &L_then, JMP_EQ);
        t->rhs->visit(this);
        Jmp j2(this, &L_then, JMP_EQ);
        Jmp j3(this, &L_out, JMP);
        L_then();
        node->block->visit(this);
        L_continue();
        node->post->visit(this);
        Jmp j4(this, &L_cond, JMP);
        L_out();

        this->continuePoint = nullptr;
        this->breakPoint = nullptr;
        return;
    }

    /**
     * <init>
     * cond:
     * <cond>
     * jmp_ne out
     * <block>
     * continue:
     * <post>
     * jmp cond
     * out:
     */
    node->init->visit(this);

    Label L_cond(this);
    Label L_out(this);
    Label L_continue(this);
    this->continuePoint = &L_continue;
    this->breakPoint = &L_out;

    node->cond->visit(this);
    Jmp j1(this, &L_out, JMP_NE);
    node->block->visit(this);
    L_continue();
    node->post->visit(this);
    Jmp j2(this, &L_cond, JMP);
    L_out();

    this->continuePoint = nullptr;
    this->breakPoint = nullptr;
}

void BytecodeGenerator::visitForEachStmt(ForEachStmt *node) {
    std::string prefix("__v");
    prefix += std::to_string(int(*(int *) node));

    // create iterator variable as iter
    std::string iter = node->identName;
    bytecode->bytecodes[bci++] = CONST_NULL;
    varNew(iter);
    // create index variable as i
    std::string index = prefix += "i";
    bytecode->bytecodes[bci++] = CONST_NULL;
    varNew(index);
    // i = 0
    constInt(0);
    varStore(localMap[index]);

    // <new array>
    node->list->visit(this);

    // cond:
    Label L_cond(this);
    Label L_out(this);
    this->continuePoint = &L_cond;
    this->breakPoint = &L_out;

    // get array length
    bytecode->bytecodes[bci++] = DUP;
    bytecode->bytecodes[bci++] = ARR_LEN;
    // compare index and array length
    varLoad(localMap[index]);
    bytecode->bytecodes[bci++] = TEST_EQ;
    // if not equal, go outside
    Jmp j1(this, &L_out, JMP_NE);
    // load array[index], and assign to iter
    bytecode->bytecodes[bci++] = DUP;
    varLoad(localMap[index]);
    bytecode->bytecodes[bci++] = LOAD_INDEX;
    varStore(localMap[iter]);
    node->block->visit(this);

    varLoad(localMap[index]);
    varLoad(localMap[index]);
    constInt(1);
    bytecode->bytecodes[bci++] = ADD;
    varStore(localMap[index]);
    // conditional checking
    Jmp j2(this, &L_cond, JMP);
    L_out();
}

void BytecodeGenerator::visitMatchStmt(MatchStmt *node) {
    // Variant 1:
    // match{} or match(cond){}
    if (node->matches.empty()) {
        return;
    }

    // Variant 2:
    // match(cond) {
    //      a=> ...
    //      b=> ...
    // }
    if (node->cond != nullptr) {
        /**
         * <cond>
         *
         * block_1:
         * dup
         * <match_1>
         * test_eq
         * jmp_ne block_2
         * <block_1>
         * jmp out
         *
         * block_2:
         * dup
         * <match_2>
         * test_eq
         * jmp_ne block_3
         * <block_2>
         * jmp out
         *
         * ...
         * out:
         */
        Label L_out(this);
        Label *L_blocks = new Label[node->matches.size()];
        for (int i = 0; i < node->matches.size(); i++) {
            L_blocks[i].setGenerator(this);
        }

        node->cond->visit(this);
        for (int i = 0; i < node->matches.size(); i++) {
            auto&[match, block, matchAll]= node->matches[i];
            if (!matchAll) {
                L_blocks[i]();
                bytecode->bytecodes[bci++] = DUP;
                match->visit(this);
                bytecode->bytecodes[bci++] = TEST_EQ;
                if (i == node->matches.size() - 1) {
                    Jmp j1(this, &L_out, JMP_NE);
                } else {
                    Jmp j1(this, &L_blocks[i + 1], JMP_NE);
                }
                block->visit(this);
                Jmp j2(this, &L_out, JMP);
            } else {
                L_blocks[i]();
                block->visit(this);
                Jmp j1(this, &L_out, JMP);
            }
        }
        L_out();
        delete[] L_blocks;
    }

    // Variant 3:
    // match{
    //      expr=> ...
    //      expr=> ...
    // }
    if (node->cond == nullptr) {
        /**
         * block_1:
         * <match_1>
         * jmp_ne block_2
         * <block_1>
         * jmp out
         *
         * block_2:
         * <match_2>
         * jmp_ne block_3
         * <block_2>
         * jmp out
         *
         * ...
         * out:
         */
        Label L_out(this);
        Label *L_blocks = new Label[node->matches.size()];
        for (int i = 0; i < node->matches.size(); i++) {
            L_blocks[i].setGenerator(this);
        }

        for (int i = 0; i < node->matches.size(); i++) {
            auto&[match, block, matchAll]= node->matches[i];
            if (!matchAll) {
                L_blocks[i]();
                match->visit(this);
                if (i == node->matches.size() - 1) {
                    Jmp j1(this, &L_out, JMP_NE);
                } else {
                    Jmp j1(this, &L_blocks[i + 1], JMP_NE);
                }
                block->visit(this);
                Jmp j2(this, &L_out, JMP);
            } else {
                L_blocks[i]();
                block->visit(this);
                Jmp j1(this, &L_out, JMP);
            }
        }
        L_out();
        delete[] L_blocks;
    }
}

void BytecodeGenerator::visitImportStmt(ImportStmt *node) {
    // Pass
}

void BytecodeGenerator::visitExportStmt(ExportStmt *node) {
    //Pass
}

void BytecodeGenerator::constInt(nyx::int32 integer) {
    bytecode->bytecodes[bci++] = CONST_I;
    *(nyx::int32 *) (bytecode->bytecodes + bci) = integer;
    bci += 4;
}

void BytecodeGenerator::constStr(const std::string &str) {
    bytecode->strings.push_back(str);
    bytecode->bytecodes[bci++] = CONST_STR;
    bytecode->bytecodes[bci++] = bytecode->strings.size() - 1;
}

void BytecodeGenerator::varLoad(int localIndex) {
    bytecode->bytecodes[bci++] = LOAD;
    bytecode->bytecodes[bci++] = localIndex;
}

void BytecodeGenerator::varStore(int localIndex) {
    bytecode->bytecodes[bci++] = STORE;
    bytecode->bytecodes[bci++] = localIndex;
}

void BytecodeGenerator::varNew(const std::string &name) {
    bytecode->bytecodes[bci++] = STORE;
    int localIndex = local++;
    localMap.insert({name, localIndex});
    bytecode->bytecodes[bci++] = localIndex;
}


bool BytecodeGenerator::isShortCircuitOr(Expr *expr) {
    return typeid(*expr) == typeid(BinaryExpr) && dynamic_cast<BinaryExpr *>(expr)->opt == TK_LOGOR;
}

bool BytecodeGenerator::isShortCircuitAnd(Expr *expr) {
    return typeid(*expr) == typeid(BinaryExpr) && dynamic_cast<BinaryExpr *>(expr)->opt == TK_LOGAND;
}

BytecodeGenerator::BytecodeGenerator() {
    this->bytecode = new Bytecode;
    this->bci = 0;
    this->local = 0;
}

void BytecodeGenerator::fixupBytecode(const std::string &funcName) {
    bytecode->bytecodeSize = bci;
    bytecode->localSize = local;
    bytecode->funcName = funcName;
}

Bytecode *BytecodeGenerator::generate(CompilationUnit *unit) {
    {
        // Generate bytecode via visitor pattern
        PhaseTime timer("generate bytecode from Ast");
        unit->visit(this);
    }
    fixupBytecode("<top-level>");
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
    fixupBytecode(node->funcName);
    // don't delete node, it's responsibility of public generate() API
    return bytecode;
}


