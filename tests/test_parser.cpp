#include <iostream>
#include <vector>
#include <string>
#include "../src/grammar/Grammar.h"
#include "../src/parser/SLRTable.h"
#include "../src/parser/Parser.h"

int main() {
    // Load grammar
    Grammar grammar;
    grammar.loadFromFile("D:/slr-parser/grammars/small.g");
    grammar.computeFirstSets();
    grammar.computeFollowSets();

    // Build SLR table
    SLRTable table(grammar);
    table.build();

    // Create parser
    Parser parser(table);

    // Test 1: valid input  →  id + num
    std::cout << "\n=== TEST 1: id + num ===\n";
    std::vector<std::string> input1 = {"id", "+", "num"};
    parser.parse(input1);

    // Test 2: valid input  →  id + id + num
    std::cout << "\n=== TEST 2: id + id + num ===\n";
    std::vector<std::string> input2 = {"id", "+", "id", "+", "num"};
    parser.parse(input2);

    // Test 3: invalid input  →  + id
    std::cout << "\n=== TEST 3: + id (invalid) ===\n";
    std::vector<std::string> input3 = {"+", "id"};
    parser.parse(input3);

    return 0;
}