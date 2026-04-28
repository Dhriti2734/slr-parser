#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include "lr0.h"
#include "../grammar/grammar.h"

struct Action {
    std::string type;   
    int         value;  

    Action() : type("error"), value(-1) {}
    Action(std::string t, int v) : type(t), value(v) {}
};

class SLRTable {
public:
    Grammar&   grammar;
    LR0Builder lr0;

    std::map<int, std::map<std::string, Action> > actionTable;

    std::map<int, std::map<std::string, int> >& gotoTable;

    std::vector<std::string> conflicts;

    SLRTable(Grammar& g) : grammar(g), lr0(g), gotoTable(lr0.gotoTable) {}

    void build();

    void printActionTable();
    void printGotoTable();
    void printSLRTable();

private:
    void fillTables();
};
