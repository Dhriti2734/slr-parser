#include <iostream>
#include <vector>
#include <string>
#include "lexer/Lexer.h"
#include "grammar/Grammar.h"
#include "parser/SLRTable.h"
#include "parser/Parser.h"

int main() {

    // ── LEXER ──
    std::string code = "int x = 10 + 20 ;";
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    std::cout << "=============================\n";
    std::cout << "   LEXER OUTPUT\n";
    std::cout << "=============================\n";
    for (auto& tok : tokens) {
        if (tok.type == TokenType::END_OF_FILE) break;
        std::cout << tok.value << "\n";
    }

    // ── GRAMMAR ──
    Grammar grammar;
    grammar.loadFromFile("D:/slr-parser/src/grammar/small.g");
    grammar.printRules();
    grammar.printTerminalsAndNonTerminals();
    grammar.computeFirstSets();
    grammar.printFirstSets();
    grammar.computeFollowSets();
    grammar.printFollowSets();

    // ── SLR TABLE ──
    SLRTable table(grammar);
    table.build();
    table.printStates();
    table.printActionTable();
    table.printGotoTable();

    // ── PARSER ──
    Parser parser(table);
    std::cout << "\n=== TEST: id + num ===\n";
    std::vector<std::string> input = {"id", "+", "num"};
    parser.parse(input);

    return 0;
}