#include "BytecodeGenerator.h"
#include "Bytecode.h"
#include "Opcode.h"

class Label;

class Jmp {
  private:
    int patching;

  public:
    explicit Jmp(BytecodeGenerator* gen, Label* label, Opcode::Mnemonic opcode);

    [[nodiscard]] int getPatching() const { return patching; }
};

class Label {
  private:
    int destination;
    BytecodeGenerator* gen;
    std::vector<Jmp> allJump;

  public:
    explicit Label(BytecodeGenerator* gen);

    void operator()();

    void addJump(Jmp jmp) { allJump.push_back(jmp); }

    void setGenerator(BytecodeGenerator* gen);

    ~Label();

    Label();
};

Label::~Label() {
    for (auto& i : allJump) {
        gen->bytecode->code[i.getPatching()] = destination;
    }
}

Label::Label(BytecodeGenerator* gen) {
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

void Label::setGenerator(BytecodeGenerator* gen) {
    this->gen = gen;
    this->destination = gen->bci;
}

Jmp::Jmp(BytecodeGenerator* gen, Label* label, Opcode::Mnemonic opcode) {
    // Generate JMP* bytecode
    gen->bytecode->code[gen->bci++] = opcode;
    // Record JMP* target as patching address and temporarily set to -1
    this->patching = gen->bci;
    gen->bytecode->code[gen->bci++] = -1;
    label->addJump(*this);
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
    genConstI(node->literal);
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
        node->lhs->visit(this);
        switch (node->opt) {
        case TK_MINUS:
            bytecode->code[bci++] = Opcode::NEG;
            break;
        case TK_LOGNOT:
            genConstI(1);
            bytecode->code[bci++] = Opcode::TEST_NE;
            break;
        case TK_BITNOT:
            bytecode->code[bci++] = Opcode::NOT;
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
            bytecode->code[bci++] = Opcode::OR;
            break;
        case TK_BITAND:
            node->lhs->visit(this);
            node->rhs->visit(this);
            bytecode->code[bci++] = Opcode::AND;
            break;
        case TK_LOGOR:
        case TK_LOGAND:
            panic("should be specially handled in control flow statements");
        case TK_EQ:
            node->lhs->visit(this);
            node->rhs->visit(this);
            bytecode->code[bci++] = Opcode::TEST_EQ;
            break;
        case TK_NE:
            node->lhs->visit(this);
            node->rhs->visit(this);
            bytecode->code[bci++] = Opcode::TEST_NE;
            break;
        case TK_GT:
            node->lhs->visit(this);
            node->rhs->visit(this);
            bytecode->code[bci++] = Opcode::TEST_GT;
            break;
        case TK_GE:
            node->lhs->visit(this);
            node->rhs->visit(this);
            bytecode->code[bci++] = Opcode::TEST_GE;
            break;
        case TK_LT:
            node->lhs->visit(this);
            node->rhs->visit(this);
            bytecode->code[bci++] = Opcode::TEST_LT;
            break;
        case TK_LE:
            node->lhs->visit(this);
            node->rhs->visit(this);
            bytecode->code[bci++] = Opcode::TEST_LE;
            break;
        case TK_PLUS:
            node->lhs->visit(this);
            node->rhs->visit(this);
            bytecode->code[bci++] = Opcode::ADD;
            break;
        case TK_MINUS:
            node->lhs->visit(this);
            node->rhs->visit(this);
            bytecode->code[bci++] = Opcode::SUB;
            break;
        case TK_MOD:
            node->lhs->visit(this);
            node->rhs->visit(this);
            bytecode->code[bci++] = Opcode::REM;
            break;
        case TK_TIMES:
            node->lhs->visit(this);
            node->rhs->visit(this);
            bytecode->code[bci++] = Opcode::MUL;
            break;
        case TK_DIV:
            node->lhs->visit(this);
            node->rhs->visit(this);
            bytecode->code[bci++] = Opcode::DIV;
            break;
        default:
            panic("should not reach here");
        }
    }
}

void BytecodeGenerator::visitFuncCallExpr(FuncCallExpr* node) {
    if (!node->funcName.empty()) {
        // normal function calling
        genConstStr(node->funcName);
        for (auto* arg : node->args) {
            arg->visit(this);
        }
        bytecode->code[bci++] = Opcode::CALL;
        bytecode->code[bci++] = node->args.size();
    } else {
        bytecode->code[bci++] = Opcode::CONST_CLOSURE;
        // TODO:push upvals
        for (auto* arg : node->args) {
            arg->visit(this);
        }
        bytecode->code[bci++] = Opcode::CALL;
        bytecode->code[bci++] = node->args.size(); // TODO: +upval.size();
    }
}

void BytecodeGenerator::visitAssignExpr(AssignExpr* node) {
    // arr[index] = value
    if (typeid(*node->lhs) == typeid(IndexExpr)) {
        auto* t = dynamic_cast<IndexExpr*>(node->lhs);
        if (node->opt == TK_ASSIGN) {
            // TODO: store freeval
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
            // TODO: store freeval
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
         * cond:
         * <cond_lhs>
         * jmp_ne out
         * <cond_rhs>
         * jmp_ne out
         * <block>
         * jmp cond
         * out:
         */
        auto* t = dynamic_cast<BinaryExpr*>(node->cond);
        Label L_cond(this);
        Label L_out(this);

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
        auto* t = dynamic_cast<BinaryExpr*>(node->cond);
        Label L_cond(this);
        Label L_out(this);
        Label L_then(this);

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
    std::string prefix("__v");
    prefix += std::to_string(int(*(int*)node));

    // create iterator variable as iter
    std::string iter = node->identName;
    bytecode->code[bci++] = Opcode::CONST_NULL;
    genStore(iter);
    // create index variable as i
    std::string index = prefix += "i";
    bytecode->code[bci++] = Opcode::CONST_NULL;
    genStore(index);
    // i = 0
    genConstI(0);
    genStore(index);

    // <new array>
    node->list->visit(this);

    // cond:
    Label L_cond(this);
    Label L_out(this);
    this->continuePoint = &L_cond;
    this->breakPoint = &L_out;

    // get array length
    bytecode->code[bci++] = Opcode::DUP;
    bytecode->code[bci++] = Opcode::ARR_LEN;
    // compare index and array length
    genLoad(index);
    bytecode->code[bci++] = Opcode::TEST_EQ;
    // if not equal, go outside
    Jmp j1(this, &L_out, Opcode::JMP_NE);
    // load array[index], and assign to iter
    bytecode->code[bci++] = Opcode::DUP;
    genLoad(index);
    bytecode->code[bci++] = Opcode::LOAD_INDEX;
    genStore(iter);
    node->block->visit(this);

    genLoad(index);
    genLoad(index);
    genConstI(1);
    bytecode->code[bci++] = Opcode::ADD;
    genStore(index);
    // conditional checking
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
    // find variable in current function scope
    if (auto iter = bytecode->localMap.find(name);
            iter != bytecode->localMap.cend()) {
        int localIndex = bytecode->localMap[name];
        bytecode->code[bci++] = Opcode::LOAD;
        bytecode->code[bci++] = localIndex;
        return;
    }

    // find in current captured free variables
    for(int i=0;i<bytecode->freeVars.size();i++){
        auto* fv = bytecode->freeVars[i];
        assert(!fv->isEnclosing);
        if(fv->name==name){
            bytecode->code[bci++] = Opcode::LOAD_FREE;
            bytecode->code[bci++] = i;
            return;
        }
    }

    // find in parent function scope
    Bytecode* parent = bytecode->parent;
    while (parent != nullptr) {
        if (auto iter = parent->localMap.find(name);
                iter != parent->localMap.cend()) {
            auto* refer = new FreeVar;
            auto* referent = new FreeVar;

            refer->name = name;
            refer->isEnclosing = false;
            refer->endpoint = referent;
            refer->value.active = nullptr;
            refer->varIndex = parent->localMap[name];

            referent->name = name;
            referent->isEnclosing = true;
            referent->endpoint = refer;
            referent->value.active = nullptr;
            referent->varIndex = parent->localMap[name];

            parent->freeVars.push_back(referent);
            bytecode->freeVars.push_back(refer);

            bytecode->code[bci++] = Opcode::LOAD_FREE;
            bytecode->code[bci++] = bytecode->freeVars.size() - 1;
            return;
        }
        parent = parent->parent;
    }

    panic("variable undefined but using");
}

void BytecodeGenerator::genStore(const std::string& name) {
    if (auto iter = bytecode->localMap.find(name);
        iter != bytecode->localMap.cend()) {
        // reassign existing variable a new value
        int localIndex = bytecode->localMap[name];
        bytecode->code[bci++] = Opcode::STORE;
        bytecode->code[bci++] = localIndex;
    } else {
        // create a new variable
        int localIndex = bytecode->localMap.size();
        bytecode->code[bci++] = Opcode::STORE;
        bytecode->code[bci++] = localIndex;

        bytecode->localMap.insert({name, localIndex});
    }
}

void BytecodeGenerator::genLoadIndex(const std::string& array, Expr* index) {
    genLoad(array);
    index->visit(this);
    bytecode->code[bci++] = Opcode::LOAD_INDEX;
}

void BytecodeGenerator::genStoreIndex(const std::string& array, Expr* value,
                                      Expr* index) {
    if (auto iter = bytecode->localMap.find(array);
        iter == bytecode->localMap.cend()) {
        panic("variable undefined but using");
    }
    genLoad(array);
    value->visit(this);
    index->visit(this);
    bytecode->code[bci++] = Opcode::STORE_INDEX;
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
    // Create new BytecodeGenerator and produce bytecodes
    BytecodeGenerator gen;
    auto* funcBytecode = gen.generateFuncDef(node);
    bytecode->functions.insert({node->funcName, funcBytecode});
}

void BytecodeGenerator::visitClosureExpr(ClosureExpr* node) {
    // Create new BytecodeGenerator and produce bytecodes
    BytecodeGenerator gen;
    auto* closureBytecode = gen.generateClosureExpr(this->bytecode, node);
    bytecode->closures.insert({node->id, closureBytecode});
    bytecode->code[bci++] = Opcode::CONST_CLOSURE;
    bytecode->code[bci++] = node->id;
}

BytecodeGenerator::BytecodeGenerator() {
    this->bytecode = new Bytecode;
    this->bci = 0;
}

Bytecode* BytecodeGenerator::generateFuncDef(FuncDef* node) {
    // create name in local map, interpreter will assign arguments to these
    // parameters
    for (auto& param : node->params) {
        bytecode->localMap.insert({param, bytecode->localMap.size()});
    }

    node->block->visit(this);
    bytecode->codeSize = bci;
    bytecode->funcName = node->funcName;
    return bytecode;
}

Bytecode* BytecodeGenerator::generateClosureExpr(Bytecode* enclosing,
                                                 ClosureExpr* node) {
    // create name in local map, interpreter will assign arguments to these
    // parameters
    for (auto& param : node->params) {
        bytecode->localMap.insert({param, bytecode->localMap.size()});
    }
    this->bytecode->parent = enclosing;
    node->block->visit(this);

    bytecode->codeSize = bci;
    bytecode->funcName = "<closure>";
    return bytecode;
}

Bytecode* BytecodeGenerator::generate(CompilationUnit* unit) {
    {
        PhaseTime timer("generate bytecode from Ast");
        unit->visit(this);
    }
    bytecode->codeSize = bci;
    bytecode->funcName = "<top-level>";
    return bytecode;
}