#include "parse_tree.h"
#include <vector>

static void printNode(ParseTreeNode* node,
                      const std::string& parentIndent,
                      bool isLast,
                      bool isRoot) {
    if (!node) return;

    if (isRoot) {
        std::cout << node->label << "\n";
    } else {
        std::cout << parentIndent
                  << (isLast ? "+-- " : "+-- ")
                  << node->label << "\n";
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
        printNode(node->children[i], childIndent, last, false);
    }
}

void printParseTree(ParseTreeNode* node,
                    const std::string& prefix,
                    bool isLast) {
    (void)prefix;
    (void)isLast;
    printNode(node, "", true, true);
}
