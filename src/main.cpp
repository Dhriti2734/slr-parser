#include <iostream>
#include <vector>
#include <string>
#include "lexer/Lexer.h"
#include "grammar/Grammar.h"
#include "parser/SLRTable.h"
#include "parser/Parser.h"
// Convert Lexer tokens to grammar terminal strings
std::string tokenToTerminal(TokenType type, const std::string& value) {
    switch (type) {
        case TokenType::KW_INT:     return "int";
        case TokenType::KW_IF:      return "if";
        case TokenType::KW_WHILE:   return "while";
        case TokenType::KW_RETURN:  return "return";
        case TokenType::IDENTIFIER: return "id";
        case TokenType::INTEGER:    return "num";
        case TokenType::FLOAT:      return "num";
        case TokenType::PLUS:       return "+";
        case TokenType::MINUS:      return "-";
        case TokenType::STAR:       return "*";
        case TokenType::SLASH:      return "/";
        case TokenType::ASSIGN:     return "=";
        case TokenType::EQEQ:       return "==";
        case TokenType::LPAREN:     return "(";
        case TokenType::RPAREN:     return ")";
        case TokenType::LBRACE:     return "{";
        case TokenType::RBRACE:     return "}";
        case TokenType::SEMICOLON:  return ";";
        case TokenType::COMMA:      return ",";
        default:                    return value;
    }
}
int main() {

    // ── REAL SOURCE CODE ──
    std::string code = "10+30+20";

    // ── STEP 1: LEXER ──
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    std::cout << "=============================\n";
    std::cout << "   LEXER OUTPUT\n";
    std::cout << "=============================\n";
    for (auto& tok : tokens) {
        if (tok.type == TokenType::END_OF_FILE) break;
        std::cout << tok.value << "\n";
    }

    // ── STEP 2: CONVERT TOKENS TO TERMINAL STRINGS ──
    std::vector<std::string> terminalTokens;
    for (auto& tok : tokens) {
        if (tok.type == TokenType::END_OF_FILE) break;
        terminalTokens.push_back(tokenToTerminal(tok.type, tok.value));
    }

    std::cout << "\n=============================\n";
    std::cout << "   CONVERTED TOKENS\n";
    std::cout << "=============================\n";
    for (auto& t : terminalTokens) {
        std::cout << t << "\n";
    }

    // ── STEP 3: GRAMMAR ──
    Grammar grammar;
    grammar.loadFromFile("D:/slr-parser/src/grammar/small.g");
    grammar.computeFirstSets();
    grammar.computeFollowSets();

    // ── STEP 4: SLR TABLE ──
    SLRTable table(grammar);
    table.build();

    // ── STEP 5: PARSER ──
    Parser parser(table);
    std::cout << "\n=== PARSING: " << code << " ===\n";
    parser.parse(terminalTokens);

    return 0;
}