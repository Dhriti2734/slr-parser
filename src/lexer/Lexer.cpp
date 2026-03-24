#include "Lexer.h"
#include <unordered_map>

static std::unordered_map<std::string, TokenType> keywords = {
    {"int",    TokenType::KW_INT},
    {"if",     TokenType::KW_IF},
    {"else",   TokenType::KW_ELSE},
    {"while",  TokenType::KW_WHILE},
    {"return", TokenType::KW_RETURN},
};

Lexer::Lexer(const std::string& src)
    : source(src), pos(0), line(1), col(1) {}

char Lexer::current() {
    if (pos >= (int)source.size()) return '\0';
    return source[pos];
}

char Lexer::peek() {
    if (pos + 1 >= (int)source.size()) return '\0';
    return source[pos + 1];
}

void Lexer::advance() {
    if (pos < (int)source.size()) {
        if (source[pos] == '\n') { line++; col = 1; }
        else col++;
        pos++;
    }
}

void Lexer::skipWhitespaceAndComments() {
    while (pos < (int)source.size()) {
        if (isspace(current())) {
            advance();
        }
        else if (current() == '/' && peek() == '/') {
            while (current() != '\n' && pos < (int)source.size())
                advance();
        }
        else break;
    }
}

Token Lexer::readIdentifierOrKeyword() {
    int startCol = col;
    std::string val;
    while (isalnum(current()) || current() == '_') {
        val += current();
        advance();
    }
    TokenType type = TokenType::IDENTIFIER;
    if (keywords.count(val)) type = keywords[val];
    return {type, val, line, startCol};
}

Token Lexer::readNumber() {
    int startCol = col;
    std::string val;
    bool isFloat = false;
    while (isdigit(current())) { val += current(); advance(); }
    if (current() == '.' && isdigit(peek())) {
        isFloat = true;
        val += current(); advance();
        while (isdigit(current())) { val += current(); advance(); }
    }
    return {isFloat ? TokenType::FLOAT : TokenType::INTEGER, val, line, startCol};
}

Token Lexer::readOperatorOrDelimiter() {
    int startCol = col;
    char c = current();

    if (c == '=' && peek() == '=') { advance(); advance(); return {TokenType::EQEQ,  "==", line, startCol}; }
    if (c == '!' && peek() == '=') { advance(); advance(); return {TokenType::NEQ,   "!=", line, startCol}; }

    advance();
    switch (c) {
        case '+': return {TokenType::PLUS,      "+", line, startCol};
        case '-': return {TokenType::MINUS,     "-", line, startCol};
        case '*': return {TokenType::STAR,      "*", line, startCol};
        case '/': return {TokenType::SLASH,     "/", line, startCol};
        case '=': return {TokenType::ASSIGN,    "=", line, startCol};
        case '<': return {TokenType::LT,        "<", line, startCol};
        case '>': return {TokenType::GT,        ">", line, startCol};
        case '(': return {TokenType::LPAREN,    "(", line, startCol};
        case ')': return {TokenType::RPAREN,    ")", line, startCol};
        case '{': return {TokenType::LBRACE,    "{", line, startCol};
        case '}': return {TokenType::RBRACE,    "}", line, startCol};
        case ';': return {TokenType::SEMICOLON, ";", line, startCol};
        case ',': return {TokenType::COMMA,     ",", line, startCol};
        default:  return {TokenType::UNKNOWN, std::string(1, c), line, startCol};
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (true) {
        skipWhitespaceAndComments();
        if (pos >= (int)source.size()) break;
        char c = current();
        if (isalpha(c) || c == '_')   tokens.push_back(readIdentifierOrKeyword());
        else if (isdigit(c))           tokens.push_back(readNumber());
        else                           tokens.push_back(readOperatorOrDelimiter());
    }
    tokens.push_back({TokenType::END_OF_FILE, "", line, col});
    return tokens;
}