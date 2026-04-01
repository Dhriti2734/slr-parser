#include "SLRTable.h"
#include <iostream>

// =============================================
// BASIC HELPERS
// =============================================

std::string SLRTable::symbolAfterDot(const Item& item) {
    auto& rule = grammar.rules[item.ruleIndex];
    if (item.dotPos < rule.rhs.size())
        return rule.rhs[item.dotPos];
    return "";
}

bool SLRTable::isDotAtEnd(const Item& item) {
    return item.dotPos >= grammar.rules[item.ruleIndex].rhs.size();
}

int SLRTable::findState(const State& s) {
    for (int i = 0; i < states.size(); i++) {
        if (states[i] == s) return i;
    }
    return -1;
}

// =============================================
// CLOSURE
// =============================================

State SLRTable::closure(const State& items) {
    State result = items;
    bool changed = true;

    while (changed) {
        changed = false;

        for (auto it : result) {
            std::string sym = symbolAfterDot(it);

            if (!grammar.nonTerminals.count(sym)) continue;

            for (int i = 0; i < grammar.rules.size(); i++) {
                if (grammar.rules[i].lhs == sym) {
                    Item newItem{i, 0};

                    if (!result.count(newItem)) {
                        result.insert(newItem);
                        changed = true;
                    }
                }
            }
        }
    }
    return result;
}

// =============================================
// GOTO
// =============================================

State SLRTable::goTo(const State& items, const std::string& symbol) {
    State moved;

    for (auto it : items) {
        if (symbolAfterDot(it) == symbol) {
            moved.insert({it.ruleIndex, it.dotPos + 1});
        }
    }

    return moved.empty() ? State() : closure(moved);
}

// =============================================
// BUILD STATES
// =============================================

void SLRTable::buildStates() {
    State start;
    start.insert({0, 0});
    start = closure(start);

    states.push_back(start);

    for (int i = 0; i < states.size(); i++) {
        std::set<std::string> symbols;

        for (auto it : states[i]) {
            std::string sym = symbolAfterDot(it);
            if (!sym.empty()) symbols.insert(sym);
        }

        for (auto sym : symbols) {
            State next = goTo(states[i], sym);

            if (next.empty()) continue;

            int idx = findState(next);

            if (idx == -1) {
                states.push_back(next);
                idx = states.size() - 1;
            }

            gotoTable[i][sym] = idx;
        }
    }
}

// =============================================
// FILL TABLES
// =============================================

void SLRTable::fillTables() {
    for (int i = 0; i < states.size(); i++) {

        for (auto it : states[i]) {

            std::string sym = symbolAfterDot(it);

            // SHIFT
            if (!sym.empty() && grammar.terminals.count(sym)) {
                int next = gotoTable[i][sym];
                actionTable[i][sym] = Action("shift", next);
            }

            // REDUCE / ACCEPT
            else if (isDotAtEnd(it)) {

                std::string lhs = grammar.rules[it.ruleIndex].lhs;

                // ACCEPT
                if (it.ruleIndex == 0) {
                    actionTable[i]["$"] = Action("accept", 0);
                }

                // REDUCE
                else {
                    std::set<std::string> follow = grammar.followSets[lhs];

                    for (auto f : follow) {
                        if (actionTable[i].count(f)) {
                            std::cout << "Conflict at state " << i << " symbol " << f << "\n";
                        }
                        actionTable[i][f] = Action("reduce", it.ruleIndex);
                    }
                }
            }
        }
    }
}

// =============================================
// BUILD
// =============================================

void SLRTable::build() {
    buildStates();
    fillTables();
}

// =============================================
// PRINT FUNCTIONS (OUTSIDE BUILD ✅)
// =============================================

void SLRTable::printStates() {
    std::cout << "\n=========== LR(0) STATES ===========\n";

    for (int i = 0; i < states.size(); i++) {
        std::cout << "\nState " << i << ":\n";

        for (auto item : states[i]) {
            auto& rule = grammar.rules[item.ruleIndex];

            std::cout << "  " << rule.lhs << " -> ";

            for (int j = 0; j < rule.rhs.size(); j++) {
                if (j == item.dotPos) std::cout << ". ";
                std::cout << rule.rhs[j] << " ";
            }

            if (item.dotPos == rule.rhs.size()) std::cout << ".";
            std::cout << "\n";
        }
    }
}

void SLRTable::printActionTable() {
    std::cout << "\n=========== ACTION TABLE ===========\n";

    for (auto& row : actionTable) {
        std::cout << "State " << row.first << ":\n";

        for (auto& col : row.second) {
            std::cout << "  [" << col.first << "] = ";

            if (col.second.type == "shift")
                std::cout << "SHIFT " << col.second.value;
            else if (col.second.type == "reduce")
                std::cout << "REDUCE by rule " << col.second.value;
            else if (col.second.type == "accept")
                std::cout << "ACCEPT";

            std::cout << "\n";
        }
    }
}

void SLRTable::printGotoTable() {
    std::cout << "\n=========== GOTO TABLE ===========\n";

    for (auto& row : gotoTable) {
        std::cout << "State " << row.first << ":\n";

        for (auto& col : row.second) {
            std::cout << "  [" << col.first << "] -> State "
                      << col.second << "\n";
        }
    }
}