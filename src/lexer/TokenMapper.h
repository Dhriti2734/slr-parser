#pragma once
#include "../lexer/Token.h"
#include <string>

inline std::string mapToken(const Token& tok) {

    switch (tok.type) {
        case TokenType::IDENTIFIER: return "id";
        case TokenType::INTEGER:
        case TokenType::FLOAT: return "num";

        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::ASSIGN: return "=";
        case TokenType::LPAREN: return "(";
        case TokenType::RPAREN: return ")";
        case TokenType::SEMICOLON: return ";";
        case TokenType::COMMA: return ",";

        case TokenType::KW_INT: return "int";
        case TokenType::KW_IF: return "if";
        case TokenType::KW_WHILE: return "while";
        case TokenType::KW_RETURN: return "return";

        default: return tok.value;
    }
}