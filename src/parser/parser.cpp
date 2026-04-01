#include "Parser.h"
#include <iostream>
#include <iomanip>
#include "../lexer/TokenMapper.h"

bool Parser::parse(const std::vector<Token>& tokensInput) {

    std::vector<std::string> tokens;

    // ✅ NO HARDCODING: directly use token values
    for (auto& tok : tokensInput) {
        if (tok.type == TokenType::END_OF_FILE) break;
        tokens.push_back(mapToken(tok));
    }

    tokens.push_back("$");

    std::stack<int> stateStack;
    std::stack<std::string> symbolStack;

    stateStack.push(0);
    symbolStack.push("$");

    int pos = 0;

    std::cout << "\n=============================================\n";
    std::cout << "                PARSE TRACE\n";
    std::cout << "=============================================\n";

    std::cout << std::left
              << std::setw(25) << "STACK"
              << std::setw(25) << "INPUT"
              << std::setw(20) << "ACTION"
              << "\n";

    std::cout << "-------------------------------------------------------------\n";

    while (true) {

        int state = stateStack.top();
        std::string token = tokens[pos];

        // STACK PRINT
        std::stack<std::string> temp = symbolStack;
        std::vector<std::string> st;

        while (!temp.empty()) {
            st.push_back(temp.top());
            temp.pop();
        }

        std::string stackStr = "";
        for (int i = st.size() - 1; i >= 0; i--) {
            stackStr += st[i] + " ";
        }

        // INPUT PRINT
        std::string inputStr = "";
        for (int i = pos; i < tokens.size(); i++) {
            inputStr += tokens[i] + " ";
        }

        // ERROR
        if (!table.actionTable[state].count(token)) {
            std::cout << std::setw(25) << stackStr
                      << std::setw(25) << inputStr
                      << "ERROR\n";

            std::cout << "\n REJECTED: Invalid input\n";
            return false;
        }

        Action action = table.actionTable[state][token];

        // SHIFT
        if (action.type == "shift") {
            std::cout << std::setw(25) << stackStr
                      << std::setw(25) << inputStr
                      << "SHIFT " + std::to_string(action.value)
                      << "\n";

            symbolStack.push(token);
            stateStack.push(action.value);
            pos++;
        }

        // REDUCE
        else if (action.type == "reduce") {

            Production& rule = table.grammar.rules[action.value];

            std::string actionStr = "REDUCE " + rule.lhs + " -> ";
            for (auto& s : rule.rhs) actionStr += s + " ";

            std::cout << std::setw(25) << stackStr
                      << std::setw(25) << inputStr
                      << actionStr << "\n";

            for (int i = 0; i < rule.rhs.size(); i++) {
                stateStack.pop();
                symbolStack.pop();
            }

            symbolStack.push(rule.lhs);

            int next = table.gotoTable[stateStack.top()][rule.lhs];
            stateStack.push(next);
        }

        // ACCEPT
        else if (action.type == "accept") {
            std::cout << std::setw(25) << stackStr
                      << std::setw(25) << inputStr
                      << "ACCEPT\n";

            std::cout << "\n ACCEPTED: Input is valid\n";
            return true;
        }
    }
}