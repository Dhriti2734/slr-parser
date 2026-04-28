#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "../parse_tree/parse_tree.h"

struct ASTNode {
    std::string          label;    
    std::vector<ASTNode*> children;  

    ASTNode(const std::string& lbl) : label(lbl) {}

    ~ASTNode() {
        for (int i = 0; i < (int)children.size(); i++) {
            delete children[i];
        }
    }
};

ASTNode* buildAST(ParseTreeNode* node);

void printAST(ASTNode* node,
              const std::string& prefix = "",
              bool isLast = true);
