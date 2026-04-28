#include "ast.h"


ASTNode* buildAST(ParseTreeNode* node) {
    if (node == nullptr) return nullptr;
    if (node->isTerminal) {
        if (node->label == "(" || node->label == ")") return nullptr;
        return new ASTNode(node->label);
    }

    std::string operatorLabel = "";
    for (int i = 0; i < (int)node->children.size(); i++) {
        ParseTreeNode* child = node->children[i];
        if (child->isTerminal) {
            std::string lbl = child->label;
            if (lbl == "+" || lbl == "-" || lbl == "*" || lbl == "/") {
                operatorLabel = lbl;
                break;
            }
        }
    }

    std::vector<ASTNode*> childASTs;
    for (int i = 0; i < (int)node->children.size(); i++) {
        ParseTreeNode* child = node->children[i];

        if (child->isTerminal) {
            std::string lbl = child->label;
            if (lbl == "+" || lbl == "-" || lbl == "*" || lbl == "/") continue;
            if (lbl == "(" || lbl == ")") continue;
            childASTs.push_back(new ASTNode(lbl));
        } else {
            ASTNode* childAST = buildAST(child);
            if (childAST != nullptr) {
                childASTs.push_back(childAST);
            }
        }
    }
 
    if (!operatorLabel.empty()) {
        ASTNode* opNode = new ASTNode(operatorLabel);
        for (int i = 0; i < (int)childASTs.size(); i++) {
            opNode->children.push_back(childASTs[i]);
        }
        return opNode;
    }

    if (childASTs.size() == 1) {
        return childASTs[0];
    }

    if (childASTs.size() > 1) {
        ASTNode* result = new ASTNode(node->label);
        for (int i = 0; i < (int)childASTs.size(); i++) {
            result->children.push_back(childASTs[i]);
        }
        return result;
    }

    return nullptr;
}


// Print AST in tree format
static void printASTNode(ASTNode* node,
                         const std::string& parentIndent,
                         bool isLast,
                         bool isRoot) {
    if (!node) return;

    if (isRoot) {
        std::cout << node->label << "\n";
    } else {
        std::cout << parentIndent << "+-- " << node->label << "\n";
    }

    std::string childIndent;
    if (isRoot) {
        childIndent = "";
    } else {
        childIndent = parentIndent + (isLast ? "    " : "|   ");
    }

    int n = (int)node->children.size();
    for (int i = 0; i < n; i++) {
        bool last = (i == n - 1);
        printASTNode(node->children[i], childIndent, last, false);
    }
}

void printAST(ASTNode* node,
              const std::string& prefix,
              bool isLast) {
    (void)prefix;
    (void)isLast;
    printASTNode(node, "", true, true);
}
