#include <iostream>
#include "../src/grammar/Grammar.h"
#include "../src/parser/SLRTable.h"

int main() {
    Grammar grammar;
    grammar.loadFromFile("D:/slr-parser/src/grammar/small.g");

    grammar.printRules();
    grammar.printTerminalsAndNonTerminals();

    grammar.computeFirstSets();
    grammar.printFirstSets();

    grammar.computeFollowSets();
    grammar.printFollowSets();

    SLRTable table(grammar);
    table.build();

    table.printStates();
    table.printActionTable();
    table.printGotoTable();

    return 0;
}