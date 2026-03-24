#pragma once
#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    int pos, line, col;

    char current();
    char peek();
    void advance();
    void skipWhitespaceAndComments();

    Token readIdentifierOrKeyword();
    Token readNumber();
    Token readOperatorOrDelimiter();
};