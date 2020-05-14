#ifndef NYX_TOKEN_H
#define NYX_TOKEN_H


//===----------------------------------------------------------------------===//
// Token definitions of nyx
//===----------------------------------------------------------------------===//
enum Token {
    INVALID = 0,  // <invalid>
    TK_IDENT,     // <identifier>
    TK_EOF,       // <eof>
    LIT_INT,      // <integer>
    LIT_STR,      // <string>
    LIT_DOUBLE,   // <decimal>
    LIT_CHAR,     // <character>

    TK_BITAND,  // &
    TK_BITOR,   // |
    TK_BITNOT,  // ~
    TK_LOGAND,  // &&
    TK_LOGOR,   // ||
    TK_LOGNOT,  // !

    TK_PLUS,   // +
    TK_MINUS,  // -
    TK_TIMES,  // *
    TK_DIV,    // /
    TK_MOD,    // %

    TK_EQ,  // ==
    TK_NE,  // !=
    TK_GT,  // >
    TK_GE,  // >=
    TK_LT,  // <
    TK_LE,  // <=

    TK_ASSIGN,     // =
    TK_PLUS_AGN,   // +=
    TK_MINUS_AGN,  // -=
    TK_TIMES_AGN,  // *=
    TK_DIV_AGN,    // /=
    TK_MOD_AGN,    // %=
    TK_MATCH,      // =>
    TK_COMMA,      // ,
    TK_LPAREN,     // (
    TK_RPAREN,     // )
    TK_LBRACE,     // {
    TK_RBRACE,     // }
    TK_LBRACKET,   // [
    TK_RBRACKET,   // ]
    TK_SEMICOLON,  // ;
    TK_COLON,      // :

    KW_IF,        // if
    KW_ELSE,      // else
    KW_TRUE,      // true
    KW_FALSE,     // false
    KW_WHILE,     // while
    KW_FOR,       // for
    KW_NULL,      // null
    KW_FUNC,      // func
    KW_RETURN,    // return
    KW_BREAK,     // break
    KW_CONTINUE,  // continue
    KW_MATCH,     // match
};

inline const char *getTokenSymbol(Token token) {
    static const char *symbol[] = {
            "<invalid>",
            "<identifier>",
            "<eof>",
            "<integer>",
            "<string>",
            "<decimal>",
            "<character>",
            "&",
            "|",
            "~",
            "&&",
            "||",
            "!",
            "+",
            "-",
            "*",
            "/",
            "%",
            "==",
            "!=",
            ">",
            ">=",
            "<",
            "<=",
            "=",
            "+=",
            "-=",
            "*=",
            "/=",
            "%=",
            "=>",
            ",",
            "(",
            ")",
            "{",
            "}",
            "[",
            "]",
            ";",
            ":",
            "if",
            "else",
            "true",
            "false",
            "while",
            "for",
            "null",
            "func",
            "return",
            "break",
            "continue",
            "match"
    };
    return symbol[token];
}

#endif //NYX_TOKEN_H
