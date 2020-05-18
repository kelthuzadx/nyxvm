#include "Ast.h"

static int IntegerIncrementer = 0;

AstNode::AstNode(int line, int column) : line(line), column(column) {
    id = IntegerIncrementer++;
}

std::string AstNode::to_string() const {
    return std::move(std::string("AstNode"));
}

std::string Expr::to_string() const {
    return std::move(std::string("Expr"));
}

std::string Stmt::to_string() const {
    return std::move(std::string("Stmt"));
}

std::string Block::to_string() const {
    return std::move(std::string("Block"));
}

std::string FuncDef::to_string() const {
    std::string str("FuncDef{");
    str += funcName;
    str += "}";
    return std::move(str);
}

std::string CompilationUnit::to_string() const {
    return std::move(std::string("CompilationUnit"));
}


std::string BoolExpr::to_string() const {
    return literal ?
           std::move(std::string("BoolExpr{True}")) :
           std::move(std::string("BlockExpr{False}"));
}

std::string CharExpr::to_string() const {
    std::string str("CharExpr{");
    str += std::to_string(literal);
    str += "}";
    return std::move(str);
}

std::string NullExpr::to_string() const {
    return std::move(std::string("NullExpr"));
}

std::string IntExpr::to_string() const {
    std::string str("IntExpr{");
    str += std::to_string(literal);
    str += "}";
    return std::move(str);
}

std::string IndexExpr::to_string() const {
    std::string str("IndexExpr{");
    str += identName;
    str += "}";
    return std::move(str);
}

std::string DoubleExpr::to_string() const {
    std::string str("DoubleExpr{");
    str += std::to_string(literal);
    str += "}";
    return std::move(str);
}

std::string StringExpr::to_string() const {
    std::string str("StringExpr{");
    str += literal;
    str += "}";
    return std::move(str);
}

std::string ArrayExpr::to_string() const {
    return std::move(std::string("ArrayExpr"));
}

std::string IdentExpr::to_string() const {
    std::string str("IdentExpr{");
    str += identName;
    str += "}";
    return std::move(str);
}

std::string BinaryExpr::to_string() const {
    std::string str("BinaryExpr{");
    str += getTokenSymbol(opt);
    str += "}";
    return std::move(str);
}

std::string AssignExpr::to_string() const {
    std::string str("AssignExpr{");
    str += getTokenSymbol(opt);
    str += "}";
    return std::move(str);
}

std::string FuncCallExpr::to_string() const {
    std::string str("FuncCallExpr{");
    if (moduleName.length() != 0) {
        str += moduleName + ".";
    }
    str += funcName;
    str += "}";
    return std::move(str);
}

std::string ClosureExpr::to_string() const {
    return std::move(std::string("ClosureExpr"));
}

#define FREE_FIELD(field) delete field;

#define FREE_VEC(field) \
for(auto&val:field){\
delete val;\
}


Expr::~Expr() {}

Stmt::~Stmt() {}

Block::~Block() {
    FREE_VEC(stmts)
}

FuncDef::~FuncDef() {
    FREE_FIELD(block)
}

CompilationUnit::~CompilationUnit() {
    FREE_VEC(definitions)
    FREE_VEC(imports)
    FREE_VEC(exports)
    FREE_VEC(topStmts)
}

BoolExpr::~BoolExpr() {}

CharExpr::~CharExpr() {}

NullExpr::~NullExpr() {}

IntExpr::~IntExpr() {}

DoubleExpr::~DoubleExpr() {}

StringExpr::~StringExpr() {}

ArrayExpr::~ArrayExpr() {
    FREE_VEC(literal)
}

IdentExpr::~IdentExpr() {}

IndexExpr::~IndexExpr() {
    FREE_FIELD(index)
}

BinaryExpr::~BinaryExpr() {
    FREE_FIELD(lhs)
    FREE_FIELD(rhs)
}

FuncCallExpr::~FuncCallExpr() {
    FREE_VEC(args)
}

AssignExpr::~AssignExpr() {
    FREE_FIELD(lhs)
    FREE_FIELD(rhs)
}

ClosureExpr::~ClosureExpr() {
    FREE_FIELD(block)
}

BreakStmt::~BreakStmt() {}

ContinueStmt::~ContinueStmt() {}

SimpleStmt::~SimpleStmt() {
    FREE_FIELD(expr)
}

ReturnStmt::~ReturnStmt() {
    FREE_FIELD(retval)
}

IfStmt::~IfStmt() {
    FREE_FIELD(cond)
    FREE_FIELD(block)
    FREE_FIELD(elseBlock)
}

WhileStmt::~WhileStmt() {
    FREE_FIELD(cond)
    FREE_FIELD(block)
}

ForStmt::~ForStmt() {
    FREE_FIELD(init)
    FREE_FIELD(cond)
    FREE_FIELD(post)
    FREE_FIELD(block)
}

ForEachStmt::~ForEachStmt() {
    FREE_FIELD(list)
    FREE_FIELD(block)
}

MatchStmt::~MatchStmt() {
    FREE_FIELD(cond)
    for (auto&[a, b, c]:matches) {
        FREE_FIELD(a)
        FREE_FIELD(b)
    }
}

ImportStmt::~ImportStmt() {
    FREE_VEC(imports)
}

ExportStmt::~ExportStmt() {
    FREE_VEC(exports)
}