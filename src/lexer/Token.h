#pragma once
#include <string>

enum class TokenType {
    // Literals
    INTEGER,
    FLOAT,
    IDENTIFIER,

    // Keywords
    KW_INT,
    KW_IF,
    KW_ELSE,
    KW_WHILE,
    KW_RETURN,

    // Operators
    PLUS,        // +
    MINUS,       // -
    STAR,        // *
    SLASH,       // /
    ASSIGN,      // =
    EQEQ,        // ==
    NEQ,         // !=
    LT,          // 
    GT,          // >

    // Delimiters
    LPAREN,      // (
    RPAREN,      // )
    LBRACE,      // {
    RBRACE,      // }
    SEMICOLON,   // ;
    COMMA,       // ,

    // Special
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType   type;
    std::string value;
    int         line;
    int         col;
};