#include "BytecodeGenerator.h"
#include "../runtime/NyxVM.h"
#include "Bytecode.h"
#include "Opcode.h"

BytecodeGenerator::Label::~Label() {
    for (auto& i : allJump) {
        gen->bytecode->code[i.getPatching()] = destination;
    }
}

BytecodeGenerator::Label::Label(BytecodeGenerator* gen) {
    this->gen = gen;
    this->destination = gen->bci;
}

void BytecodeGenerator::Label::operator()() {
    // reset destination
    this->destination = gen->bci;
}

BytecodeGenerator::Label::Label() {
    this->gen = nullptr;
    this->destination = -1;
}

void BytecodeGenerator::Label::setGenerator(BytecodeGenerator* gen) {
    this->gen = gen;
    this->destination = gen->bci;
}

BytecodeGenerator::Jmp::Jmp(BytecodeGenerator* gen,
                            BytecodeGenerator::Label* label,
                            Opcode::Mnemonic opcode) {
    // Generate JMP* bytecode
    gen->bytecode->code[gen->bci++] = opcode;
    // Record JMP* target as patching address and temporarily set to -1
    this->patching = gen->bci;
    gen->bytecode->code[gen->bci++] = -1;
    label->addJump(*this);
}

BytecodeGenerator::StateMark::StateMark(BytecodeGenerator* gen) {
    this->gen = gen;
}
void BytecodeGenerator::StateMark::save() {
    this->bytecode = gen->bytecode;
    this->continuePoint = gen->continuePoint;
    this->breakPoint = gen->breakPoint;
    this->bci = gen->bci;
}
void BytecodeGenerator::StateMark::restore() {
    gen->bytecode = this->bytecode;
    gen->continuePoint = this->continuePoint;
    gen->breakPoint = this->breakPoint;
    gen->bci = this->bci;
}

void BytecodeGenerator::visitBlock(Block* node) {
    for (auto* stmt : node->stmts) {
        stmt->visit(this);
    }
}

void BytecodeGenerator::visitCompilationUnit(CompilationUnit* node) {
    for (auto* definition : node->definitions) {
        definition->visit(this);
    }
    for (auto* stmt : node->topStmts) {
        stmt->visit(this);
    }
}

void BytecodeGenerator::visitBoolExpr(BoolExpr* node) {
    genConstI(node->literal);
}

void BytecodeGenerator::visitCharExpr(CharExpr* node) {
    genConstC(node->literal);
}

void BytecodeGenerator::visitNullExpr(NullExpr* node) { genConstNull(); }

void BytecodeGenerator::visitIntExpr(IntExpr* node) {
    genConstI(node->literal);
}

void BytecodeGenerator::visitExpr(Expr* expr) {
    panic("should not reach here");
}

void BytecodeGenerator::visitDoubleExpr(DoubleExpr* node) {
    genConstD(node->literal);
}

void BytecodeGenerator::visitStringExpr(StringExpr* node) {
    genConstStr(node->literal);
}

void BytecodeGenerator::visitArrayExpr(ArrayExpr* node) {
    genArray(node->literal);
}

void BytecodeGenerator::visitIdentExpr(IdentExpr* node) {
    genLoad(node->identName);
}

void BytecodeGenerator::visitIndexExpr(IndexExpr* node) {
    genLoadIndex(node->identName, node->index);
}

void BytecodeGenerator::visitBinaryExpr(BinaryExpr* node) {
    if (node->rhs == nullptr) {
        // unary expression
        switch (node->opt) {
        case TK_MINUS:
            genUnary(node->lhs, Opcode::NEG);
            break;
        case TK_LOGNOT:
            node->lhs->visit(this);
            genConstI(1);
            bytecode->code[bci++] = Opcode::TEST_NE;
            break;
        case TK_BITNOT:
            genUnary(node->lhs, Opcode::NOT);
            break;
        default:
            panic("should not reach here");
        }
    } else {
        // binary expression
        switch (node->opt) {
        case TK_BITOR:
            genBinary(node->lhs, node->rhs, Opcode::OR);
            break;
        case TK_BITAND:
            genBinary(node->lhs, node->rhs, Opcode::AND);
            break;
        case TK_LOGOR: {
            {
                Label L_true(this);
                Label L_false(this);
                Label L_out(this);

                node->lhs->visit(this);
                Jmp j1(this, &L_true, Opcode::JMP_EQ);
                node->rhs->visit(this);
                Jmp j2(this, &L_true, Opcode::JMP_EQ);
                L_false();
                genConstI(0);
                Jmp j4(this, &L_out, Opcode::JMP);
                L_true();
                genConstI(1);
                L_out();
            }
            break;
        }
        case TK_LOGAND: {
            {
                Label L_true(this);
                Label L_false(this);
                Label L_out(this);

                node->lhs->visit(this);
                Jmp j1(this, &L_false, Opcode::JMP_NE);
                node->rhs->visit(this);
                Jmp j2(this, &L_false, Opcode::JMP_NE);
                L_true();
                genConstI(1);
                Jmp j3(this, &L_out, Opcode::JMP);
                L_false();
                genConstI(0);
                L_out();
            }
            break;
        }
        case TK_EQ:
            genBinary(node->lhs, node->rhs, Opcode::TEST_EQ);
            break;
        case TK_NE:
            genBinary(node->lhs, node->rhs, Opcode::TEST_NE);
            break;
        case TK_GT:
            genBinary(node->lhs, node->rhs, Opcode::TEST_GT);
            break;
        case TK_GE:
            genBinary(node->lhs, node->rhs, Opcode::TEST_GE);
            break;
        case TK_LT:
            genBinary(node->lhs, node->rhs, Opcode::TEST_LT);
            break;
        case TK_LE:
            genBinary(node->lhs, node->rhs, Opcode::TEST_LE);
            break;
        case TK_PLUS:
            genBinary(node->lhs, node->rhs, Opcode::ADD);
            break;
        case TK_MINUS:
            genBinary(node->lhs, node->rhs, Opcode::SUB);
            break;
        case TK_MOD:
            genBinary(node->lhs, node->rhs, Opcode::REM);
            break;
        case TK_TIMES:
            genBinary(node->lhs, node->rhs, Opcode::MUL);
            break;
        case TK_DIV:
            genBinary(node->lhs, node->rhs, Opcode::DIV);
            break;
        default:
            panic("should not reach here");
        }
    }
}

void BytecodeGenerator::visitFuncCallExpr(FuncCallExpr* node) {
    if (!node->funcName->identName.empty()) {
        node->funcName->visit(this);
    } else {
        node->closure->visit(this);
    }
    for (auto* arg : node->args) {
        arg->visit(this);
    }
    bytecode->code[bci++] = Opcode::CALL;
    bytecode->code[bci++] = node->args.size();
}

void BytecodeGenerator::visitAssignExpr(AssignExpr* node) {
    // arr[index] = value
    if (typeid(*node->lhs) == typeid(IndexExpr)) {
        auto* t = dynamic_cast<IndexExpr*>(node->lhs);
        if (node->opt == TK_ASSIGN) {
            genStoreIndex(t->identName, node->rhs, t->index);
        } else {
            // arr[index] += value
            genLoad(t->identName);
            genLoadIndex(t->identName, t->index);
            node->rhs->visit(this);
            switch (node->opt) {
            case TK_PLUS_AGN:
                bytecode->code[bci++] = Opcode::ADD;
                break;
            case TK_MINUS_AGN:
                bytecode->code[bci++] = Opcode::SUB;
                break;
            case TK_TIMES_AGN:
                bytecode->code[bci++] = Opcode::MUL;
                break;
            case TK_DIV_AGN:
                bytecode->code[bci++] = Opcode::DIV;
                break;
            case TK_MOD_AGN:
                bytecode->code[bci++] = Opcode::REM;
                break;
            default:
                panic("should not reach here");
            }
            t->index->visit(this);
            bytecode->code[bci++] = Opcode::STORE_INDEX;
        }

    }
    // var = value
    else if (typeid(*node->lhs) == typeid(IdentExpr)) {
        auto* t = dynamic_cast<IdentExpr*>(node->lhs);
        if (node->opt == TK_ASSIGN) {
            node->rhs->visit(this);
            genStore(t->identName);
        } else {
            genLoad(t->identName);
            node->rhs->visit(this);
            switch (node->opt) {
            case TK_PLUS_AGN:
                bytecode->code[bci++] = Opcode::ADD;
                break;
            case TK_MINUS_AGN:
                bytecode->code[bci++] = Opcode::SUB;
                break;
            case TK_TIMES_AGN:
                bytecode->code[bci++] = Opcode::MUL;
                break;
            case TK_DIV_AGN:
                bytecode->code[bci++] = Opcode::DIV;
                break;
            case TK_MOD_AGN:
                bytecode->code[bci++] = Opcode::REM;
                break;
            default:
                panic("should not reach here");
            }
            genStore(t->identName);
        }
    }
}

void BytecodeGenerator::visitStmt(Stmt* node) {
    panic("should not reach here");
}

void BytecodeGenerator::visitBreakStmt(BreakStmt* node) {
    Jmp j1(this, this->breakPoint, Opcode::JMP);
}

void BytecodeGenerator::visitContinueStmt(ContinueStmt* node) {
    Jmp j1(this, this->continuePoint, Opcode::JMP);
}

void BytecodeGenerator::visitSimpleStmt(SimpleStmt* node) {
    node->expr->visit(this);
}

void BytecodeGenerator::visitReturnStmt(ReturnStmt* node) {
    if (node->retval == nullptr) {
        bytecode->code[bci++] = Opcode::RETURN;
    } else {
        node->retval->visit(this);
        bytecode->code[bci++] = Opcode::RETURN_VAL;
    }
}

void BytecodeGenerator::visitIfStmt(IfStmt* node) {
    if (isShortCircuitAnd(node->cond)) {
        auto* shortCircuit = dynamic_cast<BinaryExpr*>(node->cond);
        if (node->elseBlock == nullptr) {
            Label L_out(this);

            shortCircuit->lhs->visit(this);
            Jmp j1(this, &L_out, Opcode::JMP_NE);
            shortCircuit->rhs->visit(this);
            Jmp j2(this, &L_out, Opcode::JMP_NE);
            node->block->visit(this);
            L_out();
        } else {
            Label L_out(this);
            Label L_else(this);

            shortCircuit->lhs->visit(this);
            Jmp j1(this, &L_else, Opcode::JMP_NE);
            shortCircuit->rhs->visit(this);
            Jmp j2(this, &L_else, Opcode::JMP_NE);
            node->block->visit(this);
            Jmp j3(this, &L_out, Opcode::JMP);
            L_else();
            node->elseBlock->visit(this);
            L_out();
        }
        return;
    }
    if (isShortCircuitOr(node->cond)) {
        auto* shortCircuit = dynamic_cast<BinaryExpr*>(node->cond);
        if (node->elseBlock == nullptr) {
            Label L_then(this);
            Label L_out(this);

            shortCircuit->lhs->visit(this);
            Jmp j1(this, &L_then, Opcode::JMP_EQ);
            shortCircuit->rhs->visit(this);
            Jmp j2(this, &L_then, Opcode::JMP_EQ);
            Jmp j3(this, &L_out, Opcode::JMP);
            L_then();
            node->block->visit(this);
            L_out();
        } else {
            Label L_out(this);
            Label L_else(this);
            Label L_then(this);

            shortCircuit->lhs->visit(this);
            Jmp j1(this, &L_then, Opcode::JMP_EQ);
            shortCircuit->rhs->visit(this);
            Jmp j2(this, &L_then, Opcode::JMP_EQ);
            Jmp j3(this, &L_else, Opcode::JMP);
            L_then();
            node->block->visit(this);
            Jmp j4(this, &L_out, Opcode::JMP);
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
        Jmp j1(this, &L_out, Opcode::JMP_NE);
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
        Jmp j1(this, &L_else, Opcode::JMP_NE);
        node->block->visit(this);
        Jmp j2(this, &L_out, Opcode::JMP);
        L_else();
        node->elseBlock->visit(this);
        Jmp j3(this, &L_out, Opcode::JMP);
        L_out();
    }
}

void BytecodeGenerator::visitWhileStmt(WhileStmt* node) {
    if (isShortCircuitAnd(node->cond)) {
        /**
         * continue:
         * cond:
         * <cond_lhs>
         * jmp_ne out
         * <cond_rhs>
         * jmp_ne out
         * <block>
         * jmp cond
         * break:
         * out:
         */
        auto* t = dynamic_cast<BinaryExpr*>(node->cond);
        Label L_cond(this);
        Label L_out(this);
        this->continuePoint = &L_cond;
        this->breakPoint = &L_out;

        t->lhs->visit(this);
        Jmp j1(this, &L_out, Opcode::JMP_NE);
        t->rhs->visit(this);
        Jmp j2(this, &L_out, Opcode::JMP_NE);
        node->block->visit(this);
        Jmp j3(this, &L_cond, Opcode::JMP);
        L_out();
        return;
    }
    if (isShortCircuitOr(node->cond)) {
        /**
         * continue:
         * cond:
         * <cond_lhs>
         * jmp_eq then
         * <cond_rhs>
         * jmp_ne then
         * jmp out
         * then:
         * <block>
         * jmp cond
         * break:
         * out:
         */
        auto* t = dynamic_cast<BinaryExpr*>(node->cond);
        Label L_cond(this);
        Label L_out(this);
        Label L_then(this);
        this->continuePoint = &L_cond;
        this->breakPoint = &L_out;

        t->lhs->visit(this);
        Jmp j1(this, &L_then, Opcode::JMP_EQ);
        t->rhs->visit(this);
        Jmp j2(this, &L_then, Opcode::JMP_EQ);
        Jmp j3(this, &L_out, Opcode::JMP);
        L_then();
        node->block->visit(this);
        Jmp j4(this, &L_cond, Opcode::JMP);
        L_out();
        return;
    }

    /**
     * continue:
     * cond:
     * <cond>
     * jmp_neq out
     * <block>
     * jmp cond
     * break;
     * out:
     */
    Label L_cond(this);
    Label L_out(this);
    this->continuePoint = &L_cond;
    this->breakPoint = &L_out;

    node->cond->visit(this);
    Jmp j1(this, &L_out, Opcode::JMP_NE);
    node->block->visit(this);
    Jmp j2(this, &L_cond, Opcode::JMP);
    L_out();
}

void BytecodeGenerator::visitForStmt(ForStmt* node) {
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
        auto* t = dynamic_cast<BinaryExpr*>(node->cond);
        node->init->visit(this);

        Label L_cond(this);
        Label L_out(this);
        Label L_continue(this);
        this->continuePoint = &L_continue;
        this->breakPoint = &L_out;

        t->lhs->visit(this);
        Jmp j1(this, &L_out, Opcode::JMP_NE);
        t->rhs->visit(this);
        Jmp j2(this, &L_out, Opcode::JMP_NE);
        node->block->visit(this);
        L_continue();
        node->post->visit(this);
        Jmp j3(this, &L_cond, Opcode::JMP);
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
        auto* t = dynamic_cast<BinaryExpr*>(node->cond);
        node->init->visit(this);

        Label L_cond(this);
        Label L_then(this);
        Label L_out(this);
        Label L_continue(this);
        this->continuePoint = &L_continue;
        this->breakPoint = &L_out;

        t->lhs->visit(this);
        Jmp j1(this, &L_then, Opcode::JMP_EQ);
        t->rhs->visit(this);
        Jmp j2(this, &L_then, Opcode::JMP_EQ);
        Jmp j3(this, &L_out, Opcode::JMP);
        L_then();
        node->block->visit(this);
        L_continue();
        node->post->visit(this);
        Jmp j4(this, &L_cond, Opcode::JMP);
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
    Jmp j1(this, &L_out, Opcode::JMP_NE);
    node->block->visit(this);
    L_continue();
    node->post->visit(this);
    Jmp j2(this, &L_cond, Opcode::JMP);
    L_out();

    this->continuePoint = nullptr;
    this->breakPoint = nullptr;
}

void BytecodeGenerator::visitForEachStmt(ForEachStmt* node) {
    // Before:
    //      for(i:arr){ ... }
    // After:
    //      __v235arr = arr
    //      for(__v235idx=0;__v235arr.length<__v235idx;__v235idx+=1){
    //          i = __v235arr[__v235idx]
    //          ...
    //      }
    // which __v235idx is constructed by <prefix>+<node-addr>+<suffix>

    std::string var(node->identName);
    std::string prefix("__v");
    prefix += std::to_string(int(*(int*)node));
    std::string idx(prefix + "idx");
    std::string arr(prefix + "arr");

    // <init>
    genConstI(0);
    genStore(idx);
    genConstNull();
    genStore(var);
    node->list->visit(this);
    genStore(arr);

    // <cond>
    Label L_cond(this);
    Label L_out(this);
    Label L_continue(this);
    this->continuePoint = &L_continue;
    this->breakPoint = &L_out;

    genLoad(idx);
    genLoad(arr);
    bytecode->code[bci++] = Opcode::ARR_LEN;
    bytecode->code[bci++] = Opcode::TEST_LT;
    Jmp j1(this, &L_out, Opcode::JMP_NE);

    // <block>
    genLoad(arr);
    genLoad(idx);
    bytecode->code[bci++] = Opcode::LOAD_INDEX;
    genStore(var);
    node->block->visit(this);

    // <post>
    L_continue();
    genLoad(idx);
    genConstI(1);
    bytecode->code[bci++] = Opcode::ADD;
    genStore(idx);
    Jmp j2(this, &L_cond, Opcode::JMP);

    L_out();
}

void BytecodeGenerator::visitMatchStmt(MatchStmt* node) {
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
        auto* L_blocks = new Label[node->matches.size()];
        for (int i = 0; i < node->matches.size(); i++) {
            L_blocks[i].setGenerator(this);
        }

        node->cond->visit(this);
        for (int i = 0; i < node->matches.size(); i++) {
            auto& [match, block, matchAll] = node->matches[i];
            if (!matchAll) {
                L_blocks[i]();
                bytecode->code[bci++] = Opcode::DUP;
                match->visit(this);
                bytecode->code[bci++] = Opcode::TEST_EQ;
                if (i == node->matches.size() - 1) {
                    Jmp j1(this, &L_out, Opcode::JMP_NE);
                } else {
                    Jmp j1(this, &L_blocks[i + 1], Opcode::JMP_NE);
                }
                block->visit(this);
                Jmp j2(this, &L_out, Opcode::JMP);
            } else {
                L_blocks[i]();
                block->visit(this);
                Jmp j1(this, &L_out, Opcode::JMP);
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
        auto* L_blocks = new Label[node->matches.size()];
        for (int i = 0; i < node->matches.size(); i++) {
            L_blocks[i].setGenerator(this);
        }

        for (int i = 0; i < node->matches.size(); i++) {
            auto& [match, block, matchAll] = node->matches[i];
            if (!matchAll) {
                L_blocks[i]();
                match->visit(this);
                if (i == node->matches.size() - 1) {
                    Jmp j1(this, &L_out, Opcode::JMP_NE);
                } else {
                    Jmp j1(this, &L_blocks[i + 1], Opcode::JMP_NE);
                }
                block->visit(this);
                Jmp j2(this, &L_out, Opcode::JMP);
            } else {
                L_blocks[i]();
                block->visit(this);
                Jmp j1(this, &L_out, Opcode::JMP);
            }
        }
        L_out();
        delete[] L_blocks;
    }
}

void BytecodeGenerator::visitImportStmt(ImportStmt* node) {
    // Pass
}

void BytecodeGenerator::visitExportStmt(ExportStmt* node) {
    // Pass
}

void BytecodeGenerator::genConstI(nyx::int32 integer) {
    bytecode->code[bci++] = Opcode::CONST_I;
    *(nyx::int32*)(bytecode->code + bci) = integer;
    bci += 4;
}

void BytecodeGenerator::genConstStr(const std::string& str) {
    bytecode->strings.push_back(str);
    bytecode->code[bci++] = Opcode::CONST_STR;
    bytecode->code[bci++] = bytecode->strings.size() - 1;
}

void BytecodeGenerator::genConstD(double d) {
    bytecode->code[bci++] = Opcode::CONST_D;
    *(double*)(bytecode->code + bci) = d;
    bci += 8;
}

void BytecodeGenerator::genConstC(char c) {
    bytecode->code[bci++] = Opcode::CONST_C;
    *(nyx::int8*)(bytecode->code + bci) = c;
    bci += 1;
}

void BytecodeGenerator::genConstCallable(int id, bool isNative) {
    assert(id >= 0);
    bytecode->code[bci++] = Opcode::CONST_CALLABLE;
    if (!isNative) {
        bytecode->code[bci++] = id;
    } else {
        bytecode->code[bci++] = -1 - id;
    }
}

void BytecodeGenerator::genConstNull() {
    bytecode->code[bci++] = Opcode::CONST_NULL;
}

void BytecodeGenerator::genArray(std::vector<Expr*> elems) {
    bytecode->code[bci++] = Opcode::NEW_ARR;
    bytecode->code[bci++] = elems.size();
    for (int i = 0; i < elems.size(); i++) {
        bytecode->code[bci++] = Opcode::DUP;
        elems[i]->visit(this);
        genConstI(i);
        bytecode->code[bci++] = Opcode::STORE_INDEX;
    }
}

void BytecodeGenerator::genLoad(const std::string& name) {
    // find as local variable in current function scope
    if (auto iter = bytecode->localVars.find(name);
        iter != bytecode->localVars.cend()) {
        int localIndex = bytecode->localVars[name];
        bytecode->code[bci++] = Opcode::LOAD;
        bytecode->code[bci++] = localIndex;
        return;
    }

    // find as already captured free variable in current function scope
    for (int i = 0; i < bytecode->freeVars.size(); i++) {
        auto* fv = bytecode->freeVars[i];
        if (!fv->isEnclosing) {
            if (!fv->isEnclosing && fv->name == name) {
                bytecode->code[bci++] = Opcode::LOAD_FREE;
                bytecode->code[bci++] = i;
                return;
            }
        }
    }

    // find as nyxffi function
    if (int builtinIndex = Bytecode::findBuiltinIndex(name);
        builtinIndex >= 0) {
        genConstCallable(builtinIndex, true);
        return;
    }

    // find in lexical scope chain
    Bytecode* parent = bytecode;
    while (parent != nullptr) {
        if (auto iter = parent->localVars.find(name);
            iter != parent->localVars.cend()) {
            auto* refer = new FreeVar;
            auto* referent = new FreeVar;

            refer->name = name;
            refer->isEnclosing = false;
            refer->endpoint = referent;
            refer->value.active = nullptr;
            refer->varIndex = parent->localVars[name];

            referent->name = name;
            referent->isEnclosing = true;
            referent->endpoint = refer;
            referent->value.active = nullptr;
            referent->varIndex = parent->localVars[name];

            parent->freeVars.push_back(referent);
            bytecode->freeVars.push_back(refer);

            bytecode->code[bci++] = Opcode::LOAD_FREE;
            bytecode->code[bci++] = bytecode->freeVars.size() - 1;
            return;
        } else {
            for (auto& [funcId, funcBytecode] : parent->callables) {
                if (funcBytecode->funcName == name) {
                    genConstCallable(funcId, false);
                    return;
                }
            }
        }
        parent = parent->parent;
    }

    panic("use undefined variable %s", name.c_str());
}

void BytecodeGenerator::genStore(const std::string& name) {
    // find and assign variable in current function scope
    if (auto iter = bytecode->localVars.find(name);
        iter != bytecode->localVars.cend()) {

        int localIndex = bytecode->localVars[name];
        bytecode->code[bci++] = Opcode::STORE;
        bytecode->code[bci++] = localIndex;
        return;
    }

    // find and assign in current captured free variables
    for (int i = 0; i < bytecode->freeVars.size(); i++) {
        auto* fv = bytecode->freeVars[i];
        if (!fv->isEnclosing && fv->name == name) {
            bytecode->code[bci++] = Opcode::STORE_FREE;
            bytecode->code[bci++] = i;
            return;
        }
    }

    // find and assign in parent function scope
    Bytecode* parent = bytecode->parent;
    while (parent != nullptr) {
        if (auto iter = parent->localVars.find(name);
            iter != parent->localVars.cend()) {
            auto* refer = new FreeVar;
            auto* referent = new FreeVar;

            refer->name = name;
            refer->isEnclosing = false;
            refer->endpoint = referent;
            refer->value.active = nullptr;
            refer->varIndex = parent->localVars[name];

            referent->name = name;
            referent->isEnclosing = true;
            referent->endpoint = refer;
            referent->value.active = nullptr;
            referent->varIndex = parent->localVars[name];

            parent->freeVars.push_back(referent);
            bytecode->freeVars.push_back(refer);

            bytecode->code[bci++] = Opcode::STORE_FREE;
            bytecode->code[bci++] = bytecode->freeVars.size() - 1;
            return;
        }
        parent = parent->parent;
    }

    // no variable found, create a new variable
    int localIndex = bytecode->localVars.size();
    bytecode->code[bci++] = Opcode::STORE;
    bytecode->code[bci++] = localIndex;

    bytecode->localVars.insert({name, localIndex});
}

void BytecodeGenerator::genLoadIndex(const std::string& array, Expr* index) {
    genLoad(array);
    index->visit(this);
    bytecode->code[bci++] = Opcode::LOAD_INDEX;
}

void BytecodeGenerator::genStoreIndex(const std::string& array, Expr* value,
                                      Expr* index) {
    genLoad(array);
    value->visit(this);
    index->visit(this);
    bytecode->code[bci++] = Opcode::STORE_INDEX;
}

void BytecodeGenerator::genUnary(Expr* expr, Opcode::Mnemonic opcode) {
    expr->visit(this);
    bytecode->code[bci++] = opcode;
}

void BytecodeGenerator::genBinary(Expr* expr1, Expr* expr2,
                                  Opcode::Mnemonic opcode) {
    expr1->visit(this);
    expr2->visit(this);
    bytecode->code[bci++] = opcode;
}

inline bool BytecodeGenerator::isShortCircuitOr(Expr* expr) {
    return typeid(*expr) == typeid(BinaryExpr) &&
           dynamic_cast<BinaryExpr*>(expr)->opt == TK_LOGOR;
}

inline bool BytecodeGenerator::isShortCircuitAnd(Expr* expr) {
    return typeid(*expr) == typeid(BinaryExpr) &&
           dynamic_cast<BinaryExpr*>(expr)->opt == TK_LOGAND;
}

void BytecodeGenerator::visitFuncDef(FuncDef* node) {
    StateMark mark(this);
    mark.save();
    {
        Bytecode* newBytecode = this->bytecode->callables[node->id];
        newBytecode->parent = this->bytecode;
        this->bytecode = newBytecode;
        this->bci = 0;
        // create name in local map, interpreter will assign arguments to these
        // parameters
        for (auto& param : node->params) {
            bytecode->localVars.insert({param, bytecode->localVars.size()});
        }
        node->block->visit(this);
        this->bytecode->codeSize = bci;
    }
    mark.restore();
}

void BytecodeGenerator::visitClosureExpr(ClosureExpr* node) {
    StateMark mark(this);
    mark.save();
    {
        Bytecode* oldBytecode = this->bytecode;
        Bytecode* newBytecode = new Bytecode(node->id, "<closure>");
        newBytecode->parent = oldBytecode;
        oldBytecode->callables.insert({node->id, newBytecode});
        globalCallable.insert({node->id, newBytecode});

        this->bytecode = newBytecode;
        this->bci = 0;

        for (auto& param : node->params) {
            bytecode->localVars.insert({param, bytecode->localVars.size()});
        }
        node->block->visit(this);
        this->bytecode->codeSize = bci;
    }
    mark.restore();

    // This is an expression, we should generate corresponding bytecode for it
    genConstCallable(node->id, false);
}

BytecodeGenerator::BytecodeGenerator() {
    this->bytecode = nullptr;
    this->continuePoint = nullptr;
    this->breakPoint = nullptr;
    this->bci = 0;
}

Bytecode* BytecodeGenerator::generate(CompilationUnit* unit) {
    auto* topLevelBc = new Bytecode(unit->id, "<top-level>");
    globalCallable.insert({unit->id, topLevelBc});

    for (auto& definition : unit->definitions) {
        auto* bc = new Bytecode(definition->id, definition->funcName);
        globalCallable.insert({definition->id, bc});
        topLevelBc->callables.insert({definition->id, bc});
    }

    this->bytecode = topLevelBc;
    {
        PhaseTime timer("generate bytecode from Ast");
        unit->visit(this);
    }
    bytecode->codeSize = bci;
    return bytecode;
}
