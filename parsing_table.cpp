#include "parsing_table.hpp"

#include "first_follow.hpp"

#include <algorithm>
#include <iostream>

using namespace std;

ParsingTable makeTable(const vector<Rule>& rules,
                       const SymbolSet& terminals,
                       const map<string, SymbolSet>& first,
                       const map<string, SymbolSet>& follow) {
    ParsingTable table;
    for (const Rule& rule : rules) {
        SymbolSet productionFirst = firstOfSequence(rule.second, terminals, first);
        for (const string& lookahead : productionFirst) {
            if (lookahead != EPSILON) table[{rule.first, lookahead}].push_back(rule.second);
        }
        if (productionFirst.count(EPSILON)) {
            for (const string& lookahead : follow.at(rule.first)) {
                table[{rule.first, lookahead}].push_back(rule.second);
            }
        }
    }

    for (auto& entry : table) {
        auto& alternatives = entry.second;
        vector<Production> unique;
        for (const Production& production : alternatives) {
            if (find(unique.begin(), unique.end(), production) == unique.end()) {
                unique.push_back(production);
            }
        }
        alternatives = unique;
    }
    return table;
}

string productionText(const Production& production) {
    if (production.empty()) return EPSILON;
    string result;
    for (const string& symbol : production) {
        if (!result.empty()) result += " ";
        result += symbol;
    }
    return result;
}

void printParsingTable(const vector<string>& nonterminals,
                       const SymbolSet& terminals,
                       const ParsingTable& table) {
    cout << "\nParsing table (filled entries):\n";
    for (const string& nonterminal : nonterminals) {
        for (const string& terminal : terminals) {
            const auto entry = table.find({nonterminal, terminal});
            if (entry == table.end()) continue;
            cout << "  M[" << nonterminal << ", " << terminal << "] = ";
            for (size_t i = 0; i < entry->second.size(); ++i) {
                if (i > 0) cout << " | ";
                cout << productionText(entry->second[i]);
            }
            cout << "\n";
        }
        const auto endEntry = table.find({nonterminal, END_MARKER});
        if (endEntry != table.end()) {
            cout << "  M[" << nonterminal << ", " << END_MARKER << "] = ";
            for (size_t i = 0; i < endEntry->second.size(); ++i) {
                if (i > 0) cout << " | ";
                cout << productionText(endEntry->second[i]);
            }
            cout << "\n";
        }
    }
}

bool reportConflicts(const ParsingTable& table) {
    bool isLL1 = true;
    cout << "\nParsing table conflicts:\n";
    for (const auto& entry : table) {
        if (entry.second.size() > 1) {
            isLL1 = false;
            cout << "  M[" << entry.first.first << ", " << entry.first.second << "] has "
                 << entry.second.size() << " productions\n";
        }
    }
    if (isLL1) cout << "No conflicts found.\n";
    return isLL1;
}
