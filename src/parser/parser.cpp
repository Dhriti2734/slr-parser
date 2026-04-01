#include "Parser.h"
#include <iostream>
#include <iomanip>

bool Parser::parse(std::vector<std::string> tokens) {

    // Add end of input marker
    tokens.push_back("$");

    // Stack stores state numbers
    std::stack<int> stateStack;
    // Symbol stack stores symbols
    std::stack<std::string> symbolStack;

    // Start state is 0
    stateStack.push(0);
    symbolStack.push("$");

    int inputPos = 0; // current position in token list

    std::cout << "\n=============================\n";
    std::cout << "   PARSE TRACE\n";
    std::cout << "=============================\n";
    std::cout << std::left
              << std::setw(30) << "STACK"
              << std::setw(30) << "INPUT"
              << std::setw(20) << "ACTION"
              << "\n";
    std::cout << std::string(80, '-') << "\n";

    while (true) {

        int currentState = stateStack.top();
        std::string currentToken = tokens[inputPos];

        // Print current stack
        std::stack<std::string> tempSym = symbolStack;
        std::vector<std::string> symVec;
        while (!tempSym.empty()) {
            symVec.push_back(tempSym.top());
            tempSym.pop();
        }
        std::string stackStr = "";
        for (int i = symVec.size() - 1; i >= 0; i--)
            stackStr += symVec[i] + " ";

        // Print remaining input
        std::string inputStr = "";
        for (int i = inputPos; i < (int)tokens.size(); i++)
            inputStr += tokens[i] + " ";

        // Look up action
        if (table.actionTable[currentState].find(currentToken) ==
            table.actionTable[currentState].end()) {
            // No action found = ERROR
            std::cout << std::left
                      << std::setw(30) << stackStr
                      << std::setw(30) << inputStr
                      << std::setw(20) << "ERROR"
                      << "\n";
            std::cout << "\n REJECTED Unexpected token: "
                      << currentToken << "\n";
            return false;
        }

        Action action = table.actionTable[currentState][currentToken];

        if (action.type == "shift") {
            std::cout << std::left
                      << std::setw(30) << stackStr
                      << std::setw(30) << inputStr
                      << "SHIFT " + std::to_string(action.value)
                      << "\n";

            // Push token and new state
            symbolStack.push(currentToken);
            stateStack.push(action.value);
            inputPos++; // move to next token

        } else if (action.type == "reduce") {
            Production& rule = table.grammar.rules[action.value];

            std::string actionStr = "REDUCE by " +
                                    rule.lhs + " -> ";
            for (int i = 0; i < (int)rule.rhs.size(); i++)
                actionStr += rule.rhs[i] + " ";

            std::cout << std::left
                      << std::setw(30) << stackStr
                      << std::setw(30) << inputStr
                      << actionStr << "\n";

            // Pop rhs.size() symbols from both stacks
            for (int i = 0; i < (int)rule.rhs.size(); i++) {
                stateStack.pop();
                symbolStack.pop();
            }

            // Push lhs onto symbol stack
            symbolStack.push(rule.lhs);

            // Get goto state
            int topState = stateStack.top();
            if (table.gotoTable[topState].find(rule.lhs) ==
                table.gotoTable[topState].end()) {
                std::cout << "\n REJECTED GOTO error\n";
                return false;
            }

            int gotoState = table.gotoTable[topState][rule.lhs];
            stateStack.push(gotoState);

        } else if (action.type == "accept") {
            std::cout << std::left
                      << std::setw(30) << stackStr
                      << std::setw(30) << inputStr
                      << "ACCEPT" << "\n";
            std::cout << "\n ACCEPTED Input is valid!\n";
            return true;
        }
    }
}