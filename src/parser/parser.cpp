#include "parser.h"
#include <iostream>
#include <iomanip>
#include <sstream>


void Parser::printRow(const std::string& stackStr,
                      const std::string& inputStr,
                      const std::string& action) {
    std::cout << std::left
              << std::setw(35) << stackStr
              << std::setw(30) << inputStr
              << action << "\n";
}

bool Parser::parse(std::vector<std::string> tokens) {

    tokens.push_back("$");

    std::stack<int>              stateStack;   
    std::stack<std::string>      symStack;     
    std::stack<ParseTreeNode*>   treeStack;    

    stateStack.push(0);
    symStack.push("$");

    int inputPos = 0;

    std::cout << "\n===== PARSE TRACE =====\n";
    std::cout << std::left
              << std::setw(35) << "STACK"
              << std::setw(30) << "INPUT"
              << "ACTION\n";
    std::cout << std::string(85, '-') << "\n";

    while (true) {

        int         currentState = stateStack.top();
        std::string currentToken = tokens[inputPos];

        std::stack<std::string> tempSym = symStack;
        std::vector<std::string> symVec;
        while (!tempSym.empty()) {
            symVec.push_back(tempSym.top());
            tempSym.pop();
        }
        std::string stackStr;
        for (int i = (int)symVec.size() - 1; i >= 0; i--)
            stackStr += symVec[i] + " ";

        std::string inputStr;
        for (int i = inputPos; i < (int)tokens.size(); i++)
            inputStr += tokens[i] + " ";

        if (table.actionTable[currentState].find(currentToken) ==
            table.actionTable[currentState].end()) {
            printRow(stackStr, inputStr, "ERROR");
            std::cout << "\nREJECTED - Unexpected token: '"
                      << currentToken << "'\n";
            return false;
        }

        Action action = table.actionTable[currentState][currentToken];

        //SHIFT
        if (action.type == "shift") {
            printRow(stackStr, inputStr,
                     "SHIFT " + std::to_string(action.value));

            symStack.push(currentToken);
            stateStack.push(action.value);

            ParseTreeNode* leaf = new ParseTreeNode(currentToken, true);
            treeStack.push(leaf);

            inputPos++;
        }

        //REDUCE
        else if (action.type == "reduce") {
            Production& rule = table.grammar.rules[action.value];

            std::string actionStr = "REDUCE " + rule.lhs + " ->";
            for (int i = 0; i < (int)rule.rhs.size(); i++)
                actionStr += " " + rule.rhs[i];

            printRow(stackStr, inputStr, actionStr);

            ParseTreeNode* newNode = new ParseTreeNode(rule.lhs, false);

            int rhsLen = (int)rule.rhs.size();
            std::vector<ParseTreeNode*> children(rhsLen);
            for (int i = rhsLen - 1; i >= 0; i--) {
                stateStack.pop();
                symStack.pop();
                children[i] = treeStack.top();
                treeStack.pop();
            }
            for (int i = 0; i < rhsLen; i++) {
                newNode->children.push_back(children[i]);
            }
            treeStack.push(newNode);

            symStack.push(rule.lhs);

            // GOTO to new state
            int topState = stateStack.top();
            if (table.gotoTable[topState].find(rule.lhs) ==
                table.gotoTable[topState].end()) {
                std::cout << "\nREJECTED - GOTO error on '" << rule.lhs << "'\n";
                return false;
            }
            int gotoState = table.gotoTable[topState][rule.lhs];
            stateStack.push(gotoState);
        }

        //ACCEPT
        else if (action.type == "accept") {
            printRow(stackStr, inputStr, "ACCEPT");
            std::cout << "\nACCEPTED - Input is valid!\n";

            if (!treeStack.empty()) {
                parseTreeRoot = treeStack.top();
            }
            return true;
        }
    }
}
