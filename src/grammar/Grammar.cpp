#include "Grammar.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Helper: split a string by spaces
// "Expr + Term" → ["Expr", "+", "Term"]
std::vector<std::string> splitBySpace(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string word;
    while (iss >> word) {
        tokens.push_back(word);
    }
    return tokens;
}

// Helper: trim whitespace from both ends of string
std::string trim(const std::string& str) {
    int start = 0;
    int end = str.size() - 1;
    while (start <= end && isspace(str[start])) start++;
    while (end >= start && isspace(str[end]))   end--;
    return str.substr(start, end - start + 1);
}

void Grammar::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open grammar file: " << filename << "\n";
        return;
    }

    std::string line;
    bool firstRule = true;

    while (std::getline(file, line)) {

        // Skip empty lines and comments (lines starting with #)
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        // Split by "->"
        // "Expr -> Expr + Term"  →  left="Expr"  right="Expr + Term"
        int arrowPos = line.find("->");
        if (arrowPos == std::string::npos) continue; // no arrow, skip line

        std::string lhs = trim(line.substr(0, arrowPos));
        std::string rhsStr = trim(line.substr(arrowPos + 2));

        // rhs might have multiple options separated by |
        // "Expr -> Expr + Term | Term"
        // split by |
        std::istringstream rhsStream(rhsStr);
        std::string option;

        // check if | exists
        if (rhsStr.find('|') != std::string::npos) {
            // split by |
            std::istringstream optStream(rhsStr);
            while (std::getline(optStream, option, '|')) {
                option = trim(option);
                if (option.empty()) continue;

                Production prod;
                prod.lhs = lhs;
                prod.rhs = splitBySpace(option);
                rules.push_back(prod);
            }
        } else {
            // single rule
            Production prod;
            prod.lhs = lhs;
            prod.rhs = splitBySpace(rhsStr);
            rules.push_back(prod);
        }

        // first rule's lhs is the start symbol
        if (firstRule) {
            startSymbol = lhs;
            firstRule = false;
        }
    }

    file.close();

    // Now figure out which symbols are NonTerminals
    // Any symbol that appears on the LEFT side of -> is a NonTerminal
    for (int i = 0; i < rules.size(); i++) {
        nonTerminals.insert(rules[i].lhs);
    }

    // Now figure out Terminals
    // Any symbol on the RIGHT side that is NOT a NonTerminal is a Terminal
    for (int i = 0; i < rules.size(); i++) {
        for (int j = 0; j < rules[i].rhs.size(); j++) {
            std::string sym = rules[i].rhs[j];
            if (nonTerminals.find(sym) == nonTerminals.end()) {
                // not a nonTerminal → it's a Terminal
                terminals.insert(sym);
            }
        }
    }
    // Add augmented grammar: S' -> S
Production augmented;
augmented.lhs = startSymbol + "'";
augmented.rhs.push_back(startSymbol);

// insert at beginning
rules.insert(rules.begin(), augmented);

// update start symbol
startSymbol = augmented.lhs;
}

void Grammar::printRules() {
    std::cout << "\n=============================\n";
    std::cout << "   GRAMMAR RULES\n";
    std::cout << "=============================\n";
    for (int i = 0; i < rules.size(); i++) {
        std::cout << i << ". " << rules[i].lhs << " -> ";
        for (int j = 0; j < rules[i].rhs.size(); j++) {
            std::cout << rules[i].rhs[j];
            if (j != rules[i].rhs.size() - 1) std::cout << " ";
        }
        std::cout << "\n";
    }
}

void Grammar::printTerminalsAndNonTerminals() {
    std::cout << "\n=============================\n";
    std::cout << "   NON-TERMINALS\n";
    std::cout << "=============================\n";
    std::set<std::string>::iterator it;
    for (it = nonTerminals.begin(); it != nonTerminals.end(); it++) {
        std::cout << "  " << *it << "\n";
    }

    std::cout << "\n=============================\n";
    std::cout << "   TERMINALS\n";
    std::cout << "=============================\n";
    for (it = terminals.begin(); it != terminals.end(); it++) {
        std::cout << "  " << *it << "\n";
    }

    std::cout << "\nStart Symbol: " << startSymbol << "\n";
}

// FIRST SETS


// Get FIRST set of a single symbol
std::set<std::string> Grammar::firstOfSymbol(const std::string& symbol) {

    std::set<std::string> result;

    // If symbol is a terminal → FIRST is just itself
    if (terminals.find(symbol) != terminals.end()) {
        result.insert(symbol);
        return result;
    }

    // If symbol is a nonTerminal → look at all its rules
    for (int i = 0; i < rules.size(); i++) {
        if (rules[i].lhs != symbol) continue;

        // For each rule: symbol -> X1 X2 X3 ...
        // Add FIRST(X1) to result
        // If X1 can be empty, also add FIRST(X2), and so on
        for (int j = 0; j < rules[i].rhs.size(); j++) {
            std::string sym = rules[i].rhs[j];

            if (sym == symbol) break; // avoid infinite loop on left recursion

            std::set<std::string> firstOfSym = firstOfSymbol(sym);

            // except epsilon
            for (std::set<std::string>::iterator it = firstOfSym.begin();
                 it != firstOfSym.end(); it++) {
                if (*it != "eps") result.insert(*it);
            }

            // if this symbol cannot be empty, stop
            if (firstOfSym.find("eps") == firstOfSym.end()) break;

            // if we reach the last symbol and it can be empty
            if (j == rules[i].rhs.size() - 1) result.insert("eps");
        }
    }

    return result;
}

void Grammar::computeFirstSets() {
    // Initialize empty FIRST sets for all nonTerminals
    std::set<std::string>::iterator it;
    for (it = nonTerminals.begin(); it != nonTerminals.end(); it++) {
        firstSets[*it] = std::set<std::string>();
    }
    // Also for terminals - FIRST of terminal is itself
    for (it = terminals.begin(); it != terminals.end(); it++) {
        firstSets[*it].insert(*it);
    }

    // Keep computing until nothing changes
    bool changed = true;
    while (changed) {
        changed = false;

        for (int i = 0; i < rules.size(); i++) {
            std::string lhs = rules[i].lhs;
            std::set<std::string> first = firstOfSymbol(lhs);

            // check if anything new was added
            for (std::set<std::string>::iterator it = first.begin();
                 it != first.end(); it++) {
                if (firstSets[lhs].find(*it) == firstSets[lhs].end()) {
                    firstSets[lhs].insert(*it);
                    changed = true;
                }
            }
        }
    }
}

// =============================================
// FOLLOW SETS
// =============================================

void Grammar::computeFollowSets() {
    // Initialize empty FOLLOW sets
    std::set<std::string>::iterator it;
    for (it = nonTerminals.begin(); it != nonTerminals.end(); it++) {
        followSets[*it] = std::set<std::string>();
    }

    // Start symbol always has $ in its FOLLOW set
    followSets[startSymbol].insert("$");

    // Keep computing until nothing changes
    bool changed = true;
    while (changed) {
        changed = false;

        // Go through every rule
        for (int i = 0; i < rules.size(); i++) {
            std::string lhs = rules[i].lhs;

            // Go through every symbol on the right side
            for (int j = 0; j < rules[i].rhs.size(); j++) {
                std::string sym = rules[i].rhs[j];

                // Only compute FOLLOW for nonTerminals
                if (nonTerminals.find(sym) == nonTerminals.end()) continue;

                // Look at what comes AFTER sym in this rule
                // Rule: A -> ... sym B C ...
                // FOLLOW(sym) includes FIRST(B C ...)

                // collect FIRST of everything after sym
                std::set<std::string> firstOfRest;
                bool restCanBeEmpty = true;

                for (int k = j + 1; k < rules[i].rhs.size(); k++) {
                    std::string next = rules[i].rhs[k];
                    std::set<std::string> firstOfNext = firstSets[next];

                    // add everything except eps
                    for (std::set<std::string>::iterator ft = firstOfNext.begin();
                         ft != firstOfNext.end(); ft++) {
                        if (*ft != "eps") firstOfRest.insert(*ft);
                    }

                    // if next cannot be empty, stop
                    if (firstOfNext.find("eps") == firstOfNext.end()) {
                        restCanBeEmpty = false;
                        break;
                    }
                }

                // Add FIRST(rest) to FOLLOW(sym)
                for (std::set<std::string>::iterator ft = firstOfRest.begin();
                     ft != firstOfRest.end(); ft++) {
                    if (followSets[sym].find(*ft) == followSets[sym].end()) {
                        followSets[sym].insert(*ft);
                        changed = true;
                    }
                }

                // If everything after sym can be empty (or sym is last)
                // then FOLLOW(lhs) is also in FOLLOW(sym)
                if (restCanBeEmpty) {
                    for (std::set<std::string>::iterator ft = followSets[lhs].begin();
                         ft != followSets[lhs].end(); ft++) {
                        if (followSets[sym].find(*ft) == followSets[sym].end()) {
                            followSets[sym].insert(*ft);
                            changed = true;
                        }
                    }
                }
            }
        }
    }
}

//print functions

void Grammar::printFirstSets() {
    std::cout << "\n=============================\n";
    std::cout << "   FIRST SETS\n";
    std::cout << "=============================\n";
    std::map<std::string, std::set<std::string> >::iterator it;
    for (it = firstSets.begin(); it != firstSets.end(); it++) {
        // nonTerminals
        if (nonTerminals.find(it->first) == nonTerminals.end()) continue;
        std::cout << "FIRST(" << it->first << ") = { ";
        std::set<std::string>::iterator s;
        for (s = it->second.begin(); s != it->second.end(); s++) {
            std::cout << *s << " ";
        }
        std::cout << "}\n";
    }
}

void Grammar::printFollowSets() {
    std::cout << "\n=============================\n";
    std::cout << "   FOLLOW SETS\n";
    std::cout << "=============================\n";
    std::map<std::string, std::set<std::string> >::iterator it;
    for (it = followSets.begin(); it != followSets.end(); it++) {
        std::cout << "FOLLOW(" << it->first << ") = { ";
        std::set<std::string>::iterator s;
        for (s = it->second.begin(); s != it->second.end(); s++) {
            std::cout << *s << " ";
        }
        std::cout << "}\n";
    }
}