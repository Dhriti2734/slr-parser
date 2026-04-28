#pragma once
#include <string>
#include <vector>
#include <iostream>

struct ParseTreeNode {
    std::string              label;   
    std::vector<ParseTreeNode*> children;
    bool                     isTerminal;  

    ParseTreeNode(const std::string& lbl, bool terminal = false)
        : label(lbl), isTerminal(terminal) {}

 
    ~ParseTreeNode() {
        for (int i = 0; i < (int)children.size(); i++) {
            delete children[i];
        }
    }
};

void printParseTree(ParseTreeNode* node,
                    const std::string& prefix = "",
                    bool isLast = true);
