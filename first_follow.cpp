#include "first_follow.hpp"

#include <iostream>

using namespace std;

SymbolSet firstOfSequence(const Production& sequence,
                          const SymbolSet& terminals,
                          const map<string, SymbolSet>& first) {
    SymbolSet result;
    bool allNullable = true;

    for (const string& symbol : sequence) {
        if (terminals.count(symbol)) {
            result.insert(symbol);
            allNullable = false;
            break;
        }

        const auto found = first.find(symbol);
        if (found == first.end()) {
            allNullable = false;
            break;
        }

        for (const string& item : found->second) {
            if (item != EPSILON) result.insert(item);
        }
        if (!found->second.count(EPSILON)) {
            allNullable = false;
            break;
        }
    }

    if (allNullable) result.insert(EPSILON);
    return result;
}

map<string, SymbolSet> computeFirst(const vector<string>& nonterminals,
                                    const SymbolSet& terminals,
                                    const vector<Rule>& rules) {
    map<string, SymbolSet> first;
    for (const string& nonterminal : nonterminals) first[nonterminal];

    bool changed = true;
    while (changed) {
        changed = false;
        for (const Rule& rule : rules) {
            SymbolSet addition = firstOfSequence(rule.second, terminals, first);
            const size_t oldSize = first[rule.first].size();
            first[rule.first].insert(addition.begin(), addition.end());
            changed = changed || first[rule.first].size() != oldSize;
        }
    }
    return first;
}

map<string, SymbolSet> computeFollow(const vector<string>& nonterminals,
                                     const SymbolSet& terminals,
                                     const vector<Rule>& rules,
                                     const string& start,
                                     const map<string, SymbolSet>& first) {
    map<string, SymbolSet> follow;
    for (const string& nonterminal : nonterminals) follow[nonterminal];
    follow[start].insert(END_MARKER);

    const SymbolSet nonterminalSet(nonterminals.begin(), nonterminals.end());
    bool changed = true;
    while (changed) {
        changed = false;
        for (const Rule& rule : rules) {
            for (size_t i = 0; i < rule.second.size(); ++i) {
                const string& current = rule.second[i];
                if (!nonterminalSet.count(current)) continue;

                Production suffix(rule.second.begin() + static_cast<long>(i + 1),
                                 rule.second.end());
                SymbolSet suffixFirst = firstOfSequence(suffix, terminals, first);
                const size_t oldSize = follow[current].size();

                for (const string& item : suffixFirst) {
                    if (item != EPSILON) follow[current].insert(item);
                }
                if (suffixFirst.count(EPSILON)) {
                    follow[current].insert(follow[rule.first].begin(),
                                           follow[rule.first].end());
                }

                changed = changed || follow[current].size() != oldSize;
            }
        }
    }
    return follow;
}

void printSets(const string& title, const vector<string>& nonterminals,
               const map<string, SymbolSet>& sets) {
    cout << "\n" << title << ":\n";
    for (const string& nonterminal : nonterminals) {
        cout << "  " << title << "(" << nonterminal << ") = { ";
        bool firstItem = true;
        for (const string& item : sets.at(nonterminal)) {
            if (!firstItem) cout << ", ";
            cout << item;
            firstItem = false;
        }
        cout << " }\n";
    }
}
