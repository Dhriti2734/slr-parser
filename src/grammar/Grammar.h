#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>

struct Production {
    std::string              lhs;
    std::vector<std::string> rhs;
};

class Grammar {
public:
    std::vector<Production>             rules;
    std::set<std::string>               nonTerminals;
    std::set<std::string>               terminals;
    std::string                         startSymbol;

    // FIRST and FOLLOW sets
    std::map<std::string, std::set<std::string> > firstSets;
    std::map<std::string, std::set<std::string> > followSets;

    void loadFromFile(const std::string& filename);

    void computeFirstSets();
    void computeFollowSets();

    void printRules();
    void printTerminalsAndNonTerminals();
    void printFirstSets();
    void printFollowSets();

private:
    // helper: compute FIRST of a single symbol
    std::set<std::string> firstOfSymbol(const std::string& symbol);
};