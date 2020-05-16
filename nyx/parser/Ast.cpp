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
    std::string str("IntExpr{");
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