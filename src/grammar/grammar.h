#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>

struct Production {
    std::string              lhs;  
    std::vector<std::string> rhs;  
};


class Grammar {
public:
    std::vector<Production>                          rules;
    std::set<std::string>                            nonTerminals;
    std::set<std::string>                            terminals;
    std::string                                      startSymbol;
    std::map<std::string, std::set<std::string> >   firstSets;
    std::map<std::string, std::set<std::string> >   followSets;


    void loadFromString(const std::string& text);

    std::string augmentedStart;

    void computeFirstSets();
    void computeFollowSets();
    void printRules();
    void printFirstSets();
    void printFollowSets();

private:
    std::set<std::string> firstOfSymbol(const std::string& symbol);

    std::string trim(const std::string& str);

    std::vector<std::string> splitBySpace(const std::string& str);
};
