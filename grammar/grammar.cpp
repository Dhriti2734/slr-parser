#include "grammar.h"
#include <sstream>
#include <algorithm>

std::string Grammar::trim(const std::string& str) {
    int start = 0;
    int end   = (int)str.size() - 1;
    while (start <= end && isspace(str[start])) start++;
    while (end >= start && isspace(str[end]))   end--;
    if (start > end) return "";
    return str.substr(start, end - start + 1);
}

std::vector<std::string> Grammar::splitBySpace(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string word;
    while (iss >> word) {
        tokens.push_back(word);
    }
    return tokens;
}

void Grammar::loadFromString(const std::string& text) {

    rules.clear();
    nonTerminals.clear();
    terminals.clear();
    startSymbol = "";

    std::istringstream stream(text);
    std::string line;
    bool firstRule = true;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        size_t arrowPos = line.find("->");
        if (arrowPos == std::string::npos) continue;

        std::string lhs    = trim(line.substr(0, arrowPos));
        std::string rhsStr = trim(line.substr(arrowPos + 2));

        if (lhs.empty() || rhsStr.empty()) continue;

        if (rhsStr.find('|') != std::string::npos) {
            std::istringstream optStream(rhsStr);
            std::string option;
            while (std::getline(optStream, option, '|')) {
                option = trim(option);
                if (option.empty()) continue;
                Production prod;
                prod.lhs = lhs;
                prod.rhs = splitBySpace(option);
                rules.push_back(prod);
            }
        } else {
            Production prod;
            prod.lhs = lhs;
            prod.rhs = splitBySpace(rhsStr);
            rules.push_back(prod);
        }

        if (firstRule) {
            startSymbol = lhs;
            firstRule   = false;
        }
    }

    augmentedStart = startSymbol + "'";
    Production augProd;
    augProd.lhs = augmentedStart;
    augProd.rhs.push_back(startSymbol);
    rules.insert(rules.begin(), augProd);

    for (int i = 0; i < (int)rules.size(); i++) {
        nonTerminals.insert(rules[i].lhs);
    }

    for (int i = 0; i < (int)rules.size(); i++) {
        for (int j = 0; j < (int)rules[i].rhs.size(); j++) {
            std::string sym = rules[i].rhs[j];
            if (sym == "eps" || sym == "epsilon") continue;
            if (nonTerminals.find(sym) == nonTerminals.end()) {
                terminals.insert(sym);
            }
        }
    }
}

void Grammar::printRules() {
    std::cout << "\n===== GRAMMAR RULES =====\n";
    for (int i = 1; i < (int)rules.size(); i++) {
        std::cout << (i-1) << ". " << rules[i].lhs << " -> ";
        for (int j = 0; j < (int)rules[i].rhs.size(); j++) {
            std::cout << rules[i].rhs[j];
            if (j != (int)rules[i].rhs.size() - 1) std::cout << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\nStart Symbol: " << startSymbol << "\n";
    std::cout << "\nNon-Terminals: ";
    for (std::set<std::string>::iterator it = nonTerminals.begin();
         it != nonTerminals.end(); it++) {
        if (*it == augmentedStart) continue;
        std::cout << *it << " ";
    }
    std::cout << "\nTerminals: ";
    for (std::set<std::string>::iterator it = terminals.begin();
         it != terminals.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
}

std::set<std::string> Grammar::firstOfSymbol(const std::string& symbol) {
    std::set<std::string> result;

    if (terminals.find(symbol) != terminals.end()) {
        result.insert(symbol);
        return result;
    }

    if (symbol == "eps" || symbol == "epsilon") {
        result.insert("eps");
        return result;
    }

    for (int i = 0; i < (int)rules.size(); i++) {
        if (rules[i].lhs != symbol) continue;

        for (int j = 0; j < (int)rules[i].rhs.size(); j++) {
            std::string sym = rules[i].rhs[j];

            if (sym == symbol) break;

            std::set<std::string> firstOfSym = firstOfSymbol(sym);

            for (std::set<std::string>::iterator it = firstOfSym.begin();
                 it != firstOfSym.end(); it++) {
                if (*it != "eps") result.insert(*it);
            }

            if (firstOfSym.find("eps") == firstOfSym.end()) break;

            if (j == (int)rules[i].rhs.size() - 1) result.insert("eps");
        }
    }

    return result;
}

void Grammar::computeFirstSets() {
    for (std::set<std::string>::iterator it = nonTerminals.begin();
         it != nonTerminals.end(); it++) {
        firstSets[*it] = std::set<std::string>();
    }
    for (std::set<std::string>::iterator it = terminals.begin();
         it != terminals.end(); it++) {
        firstSets[*it].insert(*it);
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < (int)rules.size(); i++) {
            std::string lhs   = rules[i].lhs;
            std::set<std::string> first = firstOfSymbol(lhs);
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

void Grammar::computeFollowSets() {
    for (std::set<std::string>::iterator it = nonTerminals.begin();
         it != nonTerminals.end(); it++) {
        followSets[*it] = std::set<std::string>();
    }

    followSets[augmentedStart].insert("$");

    followSets[startSymbol].insert("$");

    bool changed = true;
    while (changed) {
        changed = false;

        for (int i = 0; i < (int)rules.size(); i++) {
            std::string lhs = rules[i].lhs;

            for (int j = 0; j < (int)rules[i].rhs.size(); j++) {
                std::string sym = rules[i].rhs[j];

                if (nonTerminals.find(sym) == nonTerminals.end()) continue;

                std::set<std::string> firstOfRest;
                bool restCanBeEmpty = true;

                for (int k = j + 1; k < (int)rules[i].rhs.size(); k++) {
                    std::string next = rules[i].rhs[k];
                    std::set<std::string> firstOfNext = firstSets[next];

                    for (std::set<std::string>::iterator ft = firstOfNext.begin();
                         ft != firstOfNext.end(); ft++) {
                        if (*ft != "eps") firstOfRest.insert(*ft);
                    }

                    if (firstOfNext.find("eps") == firstOfNext.end()) {
                        restCanBeEmpty = false;
                        break;
                    }
                }

                for (std::set<std::string>::iterator ft = firstOfRest.begin();
                     ft != firstOfRest.end(); ft++) {
                    if (followSets[sym].find(*ft) == followSets[sym].end()) {
                        followSets[sym].insert(*ft);
                        changed = true;
                    }
                }

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

void Grammar::printFirstSets() {
    std::cout << "\n===== FIRST SETS =====\n";
    std::map<std::string, std::set<std::string> >::iterator it;
    for (it = firstSets.begin(); it != firstSets.end(); it++) {

        if (nonTerminals.find(it->first) == nonTerminals.end()) continue;
        if (it->first == augmentedStart) continue;
        std::cout << "FIRST(" << it->first << ") = { ";
        for (std::set<std::string>::iterator s = it->second.begin();
             s != it->second.end(); s++) {
            std::cout << *s << " ";
        }
        std::cout << "}\n";
    }
}

void Grammar::printFollowSets() {
    std::cout << "\n===== FOLLOW SETS =====\n";
    std::map<std::string, std::set<std::string> >::iterator it;
    for (it = followSets.begin(); it != followSets.end(); it++) {
        if (it->first == augmentedStart) continue;
        std::cout << "FOLLOW(" << it->first << ") = { ";
        for (std::set<std::string>::iterator s = it->second.begin();
             s != it->second.end(); s++) {
            std::cout << *s << " ";
        }
        std::cout << "}\n";
    }
}
