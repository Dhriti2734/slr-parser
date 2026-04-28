#pragma once
#include <string>
#include <vector>
#include <stack>
#include "slr_table.h"
#include "../parse_tree/parse_tree.h"

class Parser {
public:
    SLRTable& table;

    ParseTreeNode* parseTreeRoot;

    Parser(SLRTable& t) : table(t), parseTreeRoot(nullptr) {}

    bool parse(std::vector<std::string> tokens);

private:
    void printRow(const std::string& stackStr,
                  const std::string& inputStr,
                  const std::string& action);
};
