#include "lr0.h"
#include <iostream>

std::string LR0Builder::symbolAfterDot(const Item& item) {
    Production& rule = grammar.rules[item.ruleIndex];
    if (item.dotPos < (int)rule.rhs.size()) {
        return rule.rhs[item.dotPos];
    }
    return "";
}

bool LR0Builder::isDotAtEnd(const Item& item) {
    Production& rule = grammar.rules[item.ruleIndex];
    return item.dotPos >= (int)rule.rhs.size();
}

int LR0Builder::findState(const State& s) {
    for (int i = 0; i < (int)states.size(); i++) {
        if (states[i] == s) return i;
    }
    return -1;
}

State LR0Builder::closure(const State& items) {
    State result = items;
    bool changed = true;

    while (changed) {
        changed = false;
        State toAdd;

        for (std::set<Item>::iterator it = result.begin();
             it != result.end(); it++) {

            std::string sym = symbolAfterDot(*it);
            if (sym.empty()) continue;

            if (grammar.nonTerminals.find(sym) == grammar.nonTerminals.end())
                continue;

            for (int i = 0; i < (int)grammar.rules.size(); i++) {
                if (grammar.rules[i].lhs != sym) continue;

                Item newItem;
                newItem.ruleIndex = i;
                newItem.dotPos    = 0;

                if (result.find(newItem) == result.end() &&
                    toAdd.find(newItem) == toAdd.end()) {
                    toAdd.insert(newItem);
                    changed = true;
                }
            }
        }

        for (std::set<Item>::iterator it = toAdd.begin();
             it != toAdd.end(); it++) {
            result.insert(*it);
        }
    }

    return result;
}

State LR0Builder::goTo(const State& items, const std::string& symbol) {
    State moved;

    for (std::set<Item>::iterator it = items.begin();
         it != items.end(); it++) {

        if (symbolAfterDot(*it) == symbol) {
            Item newItem;
            newItem.ruleIndex = it->ruleIndex;
            newItem.dotPos    = it->dotPos + 1;
            moved.insert(newItem);
        }
    }

    if (moved.empty()) return State();
    return closure(moved);
}

void LR0Builder::buildStates() {
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
            std::set<std::string> symbols;
            for (std::set<Item>::iterator it = states[i].begin();
                 it != states[i].end(); it++) {
                std::string sym = symbolAfterDot(*it);
                if (!sym.empty()) symbols.insert(sym);
            }

            for (std::set<std::string>::iterator s = symbols.begin();
                 s != symbols.end(); s++) {

                State next = goTo(states[i], *s);
                if (next.empty()) continue;

                int nextIndex = findState(next);
                if (nextIndex == -1) {
                    states.push_back(next);
                    nextIndex = (int)states.size() - 1;
                    changed = true;
                }
                gotoTable[i][*s] = nextIndex;
            }
        }
    }
}

void LR0Builder::build() {
    buildStates();
}

void LR0Builder::printStates() {
    std::cout << "\n===== LR(0) ITEM SETS =====\n";

    for (int i = 0; i < (int)states.size(); i++) {
        std::cout << "\nState " << i << ":\n";

        for (std::set<Item>::iterator it = states[i].begin();
             it != states[i].end(); it++) {

            Production& rule = grammar.rules[it->ruleIndex];
            if (rule.lhs == grammar.augmentedStart) {
                std::cout << "  [augmented] " << rule.lhs << " -> ";
            } else {
                std::cout << "  " << rule.lhs << " -> ";
            }

            for (int j = 0; j <= (int)rule.rhs.size(); j++) {
                if (j == it->dotPos) std::cout << ". ";
                if (j < (int)rule.rhs.size()) std::cout << rule.rhs[j] << " ";
            }
            std::cout << "\n";
        }

        if (gotoTable.find(i) != gotoTable.end()) {
            std::map<std::string, int>::iterator gt;
            for (gt = gotoTable[i].begin(); gt != gotoTable[i].end(); gt++) {
                std::cout << "  GOTO(" << gt->first << ") = State " << gt->second << "\n";
            }
        }
    }
}
