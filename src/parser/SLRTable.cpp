#include "SLRTable.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

// =============================================
// HELPER: get symbol after dot
// Expr -> Expr • + Term  →  "+"
// =============================================
std::string SLRTable::symbolAfterDot(const Item& item) {
    Production& rule = grammar.rules[item.ruleIndex];
    if (item.dotPos < (int)rule.rhs.size()) {
        return rule.rhs[item.dotPos];
    }
    return ""; // dot is at end
}

// HELPER: is dot at end?
// Expr -> Expr + Term •  →  true

bool SLRTable::isDotAtEnd(const Item& item) {
    Production& rule = grammar.rules[item.ruleIndex];
    return item.dotPos >= (int)rule.rhs.size();
}

// HELPER: find state index
int SLRTable::findState(const State& s) {
    for (int i = 0; i < (int)states.size(); i++) {
        if (states[i] == s) return i;
    }
    return -1;
}

// STEP 1: CLOSURE
// Keep adding items until nothing new

State SLRTable::closure(const State& items) {
    State result = items;
    bool changed = true;

    while (changed) {
        changed = false;
        State toAdd;

        for (std::set<Item>::iterator it = result.begin();
             it != result.end(); it++) {

            std::string sym = symbolAfterDot(*it);
            if (sym.empty()) continue;

            // sym must be a nonTerminal
            if (grammar.nonTerminals.find(sym) == grammar.nonTerminals.end())
                continue;

            // Add all rules for sym with dot at start
            for (int i = 0; i < (int)grammar.rules.size(); i++) {
                if (grammar.rules[i].lhs == sym) {
                    Item newItem;
                    newItem.ruleIndex = i;
                    newItem.dotPos    = 0;

                    if (result.find(newItem) == result.end()) {
                        toAdd.insert(newItem);
                        changed = true;
                    }
                }
            }
        }

        // add new items to result
        for (std::set<Item>::iterator it = toAdd.begin();
             it != toAdd.end(); it++) {
            result.insert(*it);
        }
    }

    return result;
}

// STEP 2: GOTO
// Move dot past symbol and compute closure

State SLRTable::goTo(const State& items, const std::string& symbol) {
    State moved;

    for (std::set<Item>::iterator it = items.begin();
         it != items.end(); it++) {

        std::string sym = symbolAfterDot(*it);

        if (sym == symbol) {
            // move dot one step forward
            Item newItem;
            newItem.ruleIndex = it->ruleIndex;
            newItem.dotPos    = it->dotPos + 1;
            moved.insert(newItem);
        }
    }

    if (moved.empty()) return State();

    return closure(moved);
}


// STEP 3: BUILD ALL STATES

void SLRTable::buildStates() {
    // Start with augmented rule: Program' -> • Program
    // We treat rule 0 as the start rule
    Item startItem;
    startItem.ruleIndex = 0;
    startItem.dotPos    = 0;

    State startState;
    startState.insert(startItem);
    startState = closure(startState);

    states.push_back(startState);

    bool changed = true;
    while (changed) {
        changed = false;

        for (int i = 0; i < (int)states.size(); i++) {

            // collect all symbols after dots in this state
            std::set<std::string> symbols;
            for (std::set<Item>::iterator it = states[i].begin();
                 it != states[i].end(); it++) {
                std::string sym = symbolAfterDot(*it);
                if (!sym.empty()) symbols.insert(sym);
            }

            // for each symbol compute goto
            for (std::set<std::string>::iterator s = symbols.begin();
                 s != symbols.end(); s++) {

                State next = goTo(states[i], *s);
                if (next.empty()) continue;

                int nextIndex = findState(next);
                if (nextIndex == -1) {
                    // new state found!
                    states.push_back(next);
                    nextIndex = states.size() - 1;
                    changed = true;
                }

                // record in goto table
                gotoTable[i][*s] = nextIndex;
            }
        }
    }
}


// STEP 4: FILL ACTION AND GOTO TABLES

void SLRTable::fillTables() {
    for (int i = 0; i < (int)states.size(); i++) {

        for (std::set<Item>::iterator it = states[i].begin();
             it != states[i].end(); it++) {

            std::string sym = symbolAfterDot(*it);

            // CASE 1: dot before a terminal → SHIFT
            if (!sym.empty() &&
                grammar.terminals.find(sym) != grammar.terminals.end()) {

                if (gotoTable[i].find(sym) != gotoTable[i].end()) {
                    int nextState = gotoTable[i][sym];
                    actionTable[i][sym] = Action("shift", nextState);
                }
            }

            // CASE 2: dot at end → REDUCE
            else if (isDotAtEnd(*it)) {
                std::string lhs = grammar.rules[it->ruleIndex].lhs;

                // ACCEPT if it's the start rule
                if (it->ruleIndex == 0 &&
                    grammar.followSets[lhs].find("$") !=
                    grammar.followSets[lhs].end()) {
                    actionTable[i]["$"] = Action("accept", 0);
                }

                // REDUCE for every token in FOLLOW(lhs)
                std::set<std::string> follow = grammar.followSets[lhs];
                for (std::set<std::string>::iterator f = follow.begin();
                     f != follow.end(); f++) {
                    if (*f == "$" && it->ruleIndex == 0) continue;
                    // only set if not already set (avoid conflicts)
                    if (actionTable[i].find(*f) == actionTable[i].end()) {
                        actionTable[i][*f] = Action("reduce", it->ruleIndex);
                    }
                }
            }

            // CASE 3: dot before nonTerminal → already in gotoTable
        }
    }
}

// =============================================
// BUILD: call all steps
// =============================================
void SLRTable::build() {
    buildStates();
    fillTables();
}

void SLRTable::printStates() {
    std::cout << "\n=============================\n";
    std::cout << "   LR(0) STATES\n";
    std::cout << "=============================\n";

    for (int i = 0; i < (int)states.size(); i++) {
        std::cout << "\nState " << i << ":\n";

        for (std::set<Item>::iterator it = states[i].begin();
             it != states[i].end(); it++) {

            Production& rule = grammar.rules[it->ruleIndex];
            std::cout << "  " << rule.lhs << " -> ";

            for (int j = 0; j < (int)rule.rhs.size(); j++) {
                if (j == it->dotPos) std::cout << "[.] ";
                std::cout << rule.rhs[j] << " ";
            }
            if (it->dotPos == (int)rule.rhs.size()) std::cout << "[.]";
            std::cout << "\n";
        }
    }
}
// =============================================
// PRINT ACTION TABLE
// =============================================
void SLRTable::printActionTable() {
    std::cout << "\n=============================\n";
    std::cout << "   ACTION TABLE\n";
    std::cout << "=============================\n";

    std::map<int, std::map<std::string, Action> >::iterator it;
    for (it = actionTable.begin(); it != actionTable.end(); it++) {
        std::cout << "State " << it->first << ":\n";

        std::map<std::string, Action>::iterator at;
        for (at = it->second.begin(); at != it->second.end(); at++) {
            std::cout << "  [" << at->first << "] = ";
            if (at->second.type == "shift")
                std::cout << "SHIFT " << at->second.value;
            else if (at->second.type == "reduce")
                std::cout << "REDUCE by rule " << at->second.value
                          << " (" << grammar.rules[at->second.value].lhs
                          << " -> ";
            else if (at->second.type == "accept")
                std::cout << "ACCEPT";
            std::cout << "\n";
        }
    }
}

// =============================================
// PRINT GOTO TABLE
// =============================================
void SLRTable::printGotoTable() {
    std::cout << "\n=============================\n";
    std::cout << "   GOTO TABLE\n";
    std::cout << "=============================\n";

    std::map<int, std::map<std::string, int> >::iterator it;
    for (it = gotoTable.begin(); it != gotoTable.end(); it++) {
        std::cout << "State " << it->first << ":\n";

        std::map<std::string, int>::iterator gt;
        for (gt = it->second.begin(); gt != it->second.end(); gt++) {
            std::cout << "  [" << gt->first << "] -> State "
                      << gt->second << "\n";
        }
    }
}