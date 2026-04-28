#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include "../grammar/grammar.h"

struct Item {
    int ruleIndex;  
    int dotPos;     

    bool operator<(const Item& other) const {
        if (ruleIndex != other.ruleIndex) return ruleIndex < other.ruleIndex;
        return dotPos < other.dotPos;
    }
    bool operator==(const Item& other) const {
        return ruleIndex == other.ruleIndex && dotPos == other.dotPos;
    }
};

typedef std::set<Item> State;

class LR0Builder {
public:
    Grammar& grammar;

    std::vector<State> states;

    std::map<int, std::map<std::string, int> > gotoTable;

    LR0Builder(Grammar& g) : grammar(g) {}

    void build();

    void printStates();

    std::string symbolAfterDot(const Item& item);

    bool isDotAtEnd(const Item& item);

    int findState(const State& s);

    State closure(const State& items);

    State goTo(const State& items, const std::string& symbol);

private:
    void buildStates();
};
