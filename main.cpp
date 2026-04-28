
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "grammar/grammar.h"
#include "parser/lr0.h"
#include "parser/slr_table.h"
#include "parser/parser.h"
#include "parse_tree/parse_tree.h"
#include "ast/ast.h"

std::vector<std::string> tokenizeInput(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string word;
    while (iss >> word) {
        tokens.push_back(word);
    }
    return tokens;
}

std::string readAllStdin() {
    std::string result;
    std::string line;
    while (std::getline(std::cin, line)) {
        result += line + "\n";
    }
    return result;
}

int main() {

    std::string command;
    std::getline(std::cin, command);

    while (!command.empty() && isspace(command.back()))  command.pop_back();
    while (!command.empty() && isspace(command.front())) command.erase(command.begin());

    int grammarLineCount = 0;
    std::string countLine;
    std::getline(std::cin, countLine);
    grammarLineCount = std::stoi(countLine);

    std::string grammarText;
    for (int i = 0; i < grammarLineCount; i++) {
        std::string line;
        std::getline(std::cin, line);
        grammarText += line + "\n";
    }

    std::string inputString;
    std::getline(std::cin, inputString);
    while (!inputString.empty() && isspace(inputString.back()))  inputString.pop_back();
    while (!inputString.empty() && isspace(inputString.front())) inputString.erase(inputString.begin());

    Grammar grammar;
    grammar.loadFromString(grammarText);

    if (grammar.rules.empty()) {
        std::cerr << "ERROR: No grammar rules found. Check your grammar format.\n";
        std::cerr << "Expected format:  E -> E + T | T\n";
        return 1;
    }

    grammar.computeFirstSets();
    grammar.computeFollowSets();

    if (command == "grammar") {
        grammar.printRules();
    }

    else if (command == "first_follow") {
        grammar.printRules();
        grammar.printFirstSets();
        grammar.printFollowSets();
    }

    else if (command == "lr0") {
        LR0Builder lr0(grammar);
        lr0.build();
        lr0.printStates();
    }

    else if (command == "slr_table") {
        SLRTable slrTable(grammar);
        slrTable.build();
        slrTable.lr0.printStates();
        slrTable.printSLRTable();
    }

    else if (command == "parse") {
        if (inputString.empty()) {
            std::cerr << "ERROR: No input string provided for parsing\n";
            return 1;
        }

        SLRTable slrTable(grammar);
        slrTable.build();

        std::vector<std::string> tokens = tokenizeInput(inputString);

        Parser parser(slrTable);
        parser.parse(tokens);
    }

    else if (command == "parse_tree") {
        if (inputString.empty()) {
            std::cerr << "ERROR: No input string provided\n";
            return 1;
        }

        SLRTable slrTable(grammar);
        slrTable.build();

        std::vector<std::string> tokens = tokenizeInput(inputString);

        Parser parser(slrTable);
        bool accepted = parser.parse(tokens);

        if (accepted && parser.parseTreeRoot != nullptr) {
            std::cout << "\n===== PARSE TREE =====\n";
            printParseTree(parser.parseTreeRoot);
        } else {
            std::cout << "\nNo parse tree : input was rejected.\n";
        }
    }

    else if (command == "ast") {
        if (inputString.empty()) {
            std::cerr << "ERROR: No input string provided\n";
            return 1;
        }

        SLRTable slrTable(grammar);
        slrTable.build();

        std::vector<std::string> tokens = tokenizeInput(inputString);

        Parser parser(slrTable);
        bool accepted = parser.parse(tokens);

        if (accepted && parser.parseTreeRoot != nullptr) {
            std::cout << "\n===== PARSE TREE =====\n";
            printParseTree(parser.parseTreeRoot);

            std::cout << "\n===== ABSTRACT SYNTAX TREE (AST) =====\n";
            ASTNode* ast = buildAST(parser.parseTreeRoot);
            if (ast != nullptr) {
                printAST(ast);
                delete ast;
            } else {
                std::cout << "AST is empty (nothing to simplify)\n";
            }
        } else {
            std::cout << "\nNo AST : input was rejected.\n";
        }
    }

    else {
        std::cerr << "ERROR: Unknown command '" << command << "'\n";
        std::cerr << "Valid commands: grammar, first_follow, lr0, slr_table, parse, parse_tree, ast\n";
        return 1;
    }

    return 0;
}
