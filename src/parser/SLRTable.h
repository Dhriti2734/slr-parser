#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include "../grammar/Grammar.h"

// =============================================
// LR(0) ITEM
// A rule with a dot showing how far we've parsed
// Expr -> Expr • + Term
// =============================================
struct Item {
    int  ruleIndex;  // which rule this is (index into grammar.rules)
    int  dotPos;     // where the dot is (0 = before everything)

    bool operator<(const Item& other) const {
        if (ruleIndex != other.ruleIndex) return ruleIndex < other.ruleIndex;
        return dotPos < other.dotPos;
    }

    bool operator==(const Item& other) const {
        return ruleIndex == other.ruleIndex && dotPos == other.dotPos;
    }
};

// A STATE is a collection of items
typedef std::set<Item> State;

// =============================================
// ACTION TABLE ENTRY
// =============================================
struct Action {
    std::string type;   // "shift", "reduce", "accept", "error"
    int         value;  // state number (for shift) or rule index (for reduce)

    Action() : type("error"), value(-1) {}
    Action(std::string t, int v) : type(t), value(v) {}
};

// =============================================
// SLR TABLE BUILDER
// =============================================
class SLRTable {
public:
    Grammar& grammar;

    // All states (each state = set of items)
    std::vector<State> states;

    // GOTO table:   gotoTable[stateNum][symbol] = nextStateNum
    std::map<int, std::map<std::string, int> > gotoTable;

    // ACTION table: actionTable[stateNum][terminal] = Action
    std::map<int, std::map<std::string, Action> > actionTable;

    SLRTable(Grammar& g) : grammar(g) {}

    void build();
    void printStates();
    void printActionTable();
    void printGotoTable();

private:
    // Step 1 - closure of a state
    State closure(const State& items);

    // Step 2 - goto: what state do we go to after seeing symbol?
    State goTo(const State& items, const std::string& symbol);

    // Step 3 - build all states
    void buildStates();

    // Step 4 - fill action and goto tables
    void fillTables();

    // Helper: get the symbol after the dot in an item
    // Expr -> Expr • + Term   →   returns "+"
    std::string symbolAfterDot(const Item& item);

    // Helper: check if dot is at the end
    // Expr -> Expr + Term •   →   returns true
    bool isDotAtEnd(const Item& item);

    // Helper: find state index (-1 if not found)
    int findState(const State& s);
};