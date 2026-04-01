#include <iostream>
#include <vector>
#include <string>
#include "lexer/Lexer.h"
#include "grammar/Grammar.h"
#include "parser/SLRTable.h"
#include "parser/Parser.h"

int main() {

    std::string code;

    std::cout << "=====================================\n";
    std::cout << "        SLR PARSER (GENERIC)\n";
    std::cout << "=====================================\n";

    std::cout << "Enter input string: ";
    std::getline(std::cin, code);

    // LEXER
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    std::cout << "\nTOKENS: ";
    for (auto& t : tokens) {
        if (t.type == TokenType::END_OF_FILE) break;
        std::cout << t.value << " ";
    }
    std::cout << "\n";

    // GRAMMAR
    Grammar grammar;
    grammar.loadFromFile("D:/slr-parser/src/grammar/small.g");

    grammar.computeFirstSets();
    grammar.computeFollowSets();

    // TABLE
    SLRTable table(grammar);
    table.build();

    table.printStates();
    table.printActionTable();
    table.printGotoTable();

    // PARSER
    Parser parser(table);

    std::cout << "\n=========== PARSING ===========\n";
    parser.parse(tokens);

    return 0;
}