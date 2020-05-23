#ifndef NYX_PARSER_H
#define NYX_PARSER_H

#include "Ast.h"
#include "Token.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

//===----------------------------------------------------------------------===//
// Parser accepts source code and produce an Ast structure CompilationUnit which
// represents the whole source code
//===----------------------------------------------------------------------===//
class Parser {
  private:
    const std::unordered_map<std::string, Token> keywords;
    std::tuple<Token, std::string> currentToken;
    std::fstream fs;
    int line = 1;
    int column = 0;

  public:
    explicit Parser(const std::string& fileName);

    ~Parser();

  public:
    CompilationUnit* parse();

    static void dumpLex(const std::string& saveFileName,
                        const std::string& sourceFileName);

  private:
    Expr* parsePrimaryExpr();

    Expr* parseUnaryExpr();

    Expr* parseExpression(short oldPrecedence = 1);

    SimpleStmt* parseExpressionStmt();

    IfStmt* parseIfStmt();

    WhileStmt* parseWhileStmt();

    Stmt* parseForStmt();

    MatchStmt* parseMatchStmt();

    ReturnStmt* parseReturnStmt();

    ImportStmt* parseImportStmt();

    ExportStmt* parseExportStmt();

    Stmt* parseStatement();

    std::vector<Stmt*> parseStatementList();

    Block* parseBlock();

    std::vector<std::string> parseParameterList();

    FuncDef* parseFuncDef();

  private:
    static short getPrecedence(Token op);

    std::tuple<Token, std::string> next();

    inline char getNextChar() {
        column++;
        return static_cast<char>(fs.get());
    }

    inline char peekNextChar() { return static_cast<char>(fs.peek()); }

    inline Token getCurrentToken() const {
        return std::get<Token>(currentToken);
    }

    inline std::string getCurrentLexeme() const {
        return std::get<std::string>(currentToken);
    }
};

#endif
