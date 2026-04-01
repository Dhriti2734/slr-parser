#pragma once
#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include "SLRTable.h"
#include "../lexer/Token.h"

class Parser {
public:
    SLRTable& table;

    Parser(SLRTable& t) : table(t) {}

    // Main function - give it tokens, it parses
    bool parse(std::vector<std::string> tokens);

private:
    // convert token to grammar terminal string
    std::string tokenToTerminal(const std::string& token);
};