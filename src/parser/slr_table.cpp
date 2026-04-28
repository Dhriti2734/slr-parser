#include "slr_table.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>

void SLRTable::fillTables() {
    for (int i = 0; i < (int)lr0.states.size(); i++) {

        for (std::set<Item>::iterator it = lr0.states[i].begin();
             it != lr0.states[i].end(); it++) {

            std::string sym = lr0.symbolAfterDot(*it);

            if (!sym.empty() &&
                grammar.terminals.find(sym) != grammar.terminals.end()) {

                if (gotoTable[i].find(sym) != gotoTable[i].end()) {
                    int nextState = gotoTable[i][sym];
                    Action newAction("shift", nextState);

                    if (actionTable[i].find(sym) != actionTable[i].end()) {
                        std::string conflict = "CONFLICT at State " +
                            std::to_string(i) + " on '" + sym +
                            "': " + actionTable[i][sym].type + "/" + newAction.type;
                        conflicts.push_back(conflict);
                    } else {
                        actionTable[i][sym] = newAction;
                    }
                }
            }

            else if (lr0.isDotAtEnd(*it)) {
                std::string lhs = grammar.rules[it->ruleIndex].lhs;

                if (it->ruleIndex == 0) {
                    actionTable[i]["$"] = Action("accept", 0);
                    continue; // Don't also add reduce for rule 0
                }

                std::set<std::string> follow = grammar.followSets[lhs];
                for (std::set<std::string>::iterator f = follow.begin();
                     f != follow.end(); f++) {

                    Action newAction("reduce", it->ruleIndex);

                    if (actionTable[i].find(*f) != actionTable[i].end()) {
                        Action existing = actionTable[i][*f];
                        std::string conflict = "CONFLICT at State " +
                            std::to_string(i) + " on '" + *f +
                            "': " + existing.type + "/" + newAction.type +
                            " (Grammar may not be SLR(1))";
                        conflicts.push_back(conflict);
                    } else {
                        actionTable[i][*f] = newAction;
                    }
                }
            }
        }
    }
}

void SLRTable::build() {
    lr0.build();
    fillTables();
}

void SLRTable::printActionTable() {
    std::cout << "\n===== ACTION TABLE =====\n";

    std::set<std::string> allTerminals;
    for (std::set<std::string>::iterator t = grammar.terminals.begin();
         t != grammar.terminals.end(); t++) {
        allTerminals.insert(*t);
    }
    allTerminals.insert("$");

    std::cout << std::left << std::setw(8) << "State";
    for (std::set<std::string>::iterator t = allTerminals.begin();
         t != allTerminals.end(); t++) {
        std::cout << std::setw(12) << *t;
    }
    std::cout << "\n" << std::string(8 + 12 * allTerminals.size(), '-') << "\n";

    for (int i = 0; i < (int)lr0.states.size(); i++) {
        std::cout << std::setw(8) << i;
        for (std::set<std::string>::iterator t = allTerminals.begin();
             t != allTerminals.end(); t++) {
            if (actionTable[i].find(*t) != actionTable[i].end()) {
                Action& a = actionTable[i][*t];
                std::string cell;
                if (a.type == "shift")  cell = "s" + std::to_string(a.value);
                if (a.type == "reduce") cell = "r" + std::to_string(a.value);
                if (a.type == "accept") cell = "acc";
                std::cout << std::setw(12) << cell;
            } else {
                std::cout << std::setw(12) << "";
            }
        }
        std::cout << "\n";
    }

    if (!conflicts.empty()) {
        std::cout << "\n===== CONFLICTS DETECTED =====\n";
        for (int i = 0; i < (int)conflicts.size(); i++) {
            std::cout << "  " << conflicts[i] << "\n";
        }
    }
}

void SLRTable::printGotoTable() {
    std::cout << "\n===== GOTO TABLE =====\n";

    std::vector<std::string> nts;
    for (std::set<std::string>::iterator it = grammar.nonTerminals.begin();
         it != grammar.nonTerminals.end(); it++) {
        if (*it == grammar.augmentedStart) continue;
        nts.push_back(*it);
    }

    std::cout << std::left << std::setw(8) << "State";
    for (int i = 0; i < (int)nts.size(); i++) {
        std::cout << std::setw(12) << nts[i];
    }
    std::cout << "\n" << std::string(8 + 12 * nts.size(), '-') << "\n";

    for (int i = 0; i < (int)lr0.states.size(); i++) {
        std::cout << std::setw(8) << i;
        for (int j = 0; j < (int)nts.size(); j++) {
            if (gotoTable[i].find(nts[j]) != gotoTable[i].end()) {
                std::cout << std::setw(12) << gotoTable[i][nts[j]];
            } else {
                std::cout << std::setw(12) << "";
            }
        }
        std::cout << "\n";
    }
}

void SLRTable::printSLRTable() {
    printActionTable();
    printGotoTable();
}
