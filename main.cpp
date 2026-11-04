#include <bits/stdc++.h>
using namespace std;

const string EPSILON = "epsilon";
const string END_MARKER = "$";
using Production = vector<string>;
using Rule = pair<string, Production>;
using SymbolSet = set<string>;
using ParsingTable = map<pair<string, string>, vector<Production>>;

vector<string> splitWords(const string& line) {
    istringstream input(line);
    vector<string> words;
    string word;
    while (input >> word) words.push_back(word);
    return words;
}

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

ParsingTable makeTable(const vector<Rule>& rules,
                       const SymbolSet& terminals,
                       const SymbolSet& nonterminals,
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

bool parseInput(const vector<string>& tokens, const string& start,
                const SymbolSet& terminals, const SymbolSet& nonterminals,
                const ParsingTable& table) {
    vector<string> stack = {END_MARKER, start};
    size_t position = 0;

    cout << "\nParsing steps (stack top is on the right):\n";
    size_t step = 1;
    while (!stack.empty()) {
        const string top = stack.back();
        const string lookahead = tokens[position];

        cout << "Step " << step++ << ":\n";
        cout << "  Stack: ";
        for (const string& symbol : stack) cout << symbol << " ";
        cout << "\n  Input buffer: ";
        for (size_t i = position; i < tokens.size(); ++i) {
            cout << tokens[i] << " ";
        }
        cout << "\n";

        if (top == END_MARKER && lookahead == END_MARKER) {
            cout << "  Action: accept\n";
            return true;
        }

        if (terminals.count(top) || top == END_MARKER) {
            if (top != lookahead) {
                cout << "  Action: reject; expected '" << top << "' but found '"
                     << lookahead << "'.\n";
                return false;
            }
            cout << "  Action: match '" << lookahead << "' and pop it from the stack\n";
            stack.pop_back();
            ++position;
            continue;
        }

        if (!nonterminals.count(top)) {
            cout << "  Action: reject; unknown grammar symbol '" << top << "'.\n";
            return false;
        }

        const auto entry = table.find({top, lookahead});
        if (entry == table.end() || entry->second.empty()) {
            cout << "  Action: reject; no table entry for (" << top << ", "
                 << lookahead << ").\n";
            return false;
        }

        const Production& production = entry->second.front();
        cout << "  Action: expand " << top << " -> "
             << productionText(production) << "\n";
        stack.pop_back();
        for (auto it = production.rbegin(); it != production.rend(); ++it) {
            stack.push_back(*it);
        }
    }
    return false;
}

int main() {
    cout << "LL(1) Grammar Analyzer and Parser\n";
    cout << "Enter symbols separated by spaces. Use 'epsilon' for an empty "
            "production; do not include $ in the terminal list.\n";

    string line;
    cout << "\nTerminals: ";
    if (!getline(cin, line)) return 1;
    const vector<string> terminalWords = splitWords(line);
    const SymbolSet terminals(terminalWords.begin(), terminalWords.end());
    if (terminals.empty() || terminals.count(EPSILON) || terminals.count(END_MARKER)) {
        cerr << "Invalid terminal list. It must be non-empty and cannot contain "
                "'epsilon' or '$'.\n";
        return 1;
    }

    cout << "Nonterminals: ";
    if (!getline(cin, line)) return 1;
    const vector<string> nonterminals = splitWords(line);
    const SymbolSet nonterminalSet(nonterminals.begin(), nonterminals.end());
    if (nonterminals.empty()) {
        cerr << "The nonterminal list cannot be empty.\n";
        return 1;
    }
    for (const string& symbol : nonterminalSet) {
        if (terminals.count(symbol) || symbol == EPSILON || symbol == END_MARKER) {
            cerr << "Symbol lists overlap or use a reserved symbol: " << symbol << "\n";
            return 1;
        }
    }

    cout << "Start symbol: ";
    if (!getline(cin, line)) return 1;
    const vector<string> startWords = splitWords(line);
    if (startWords.size() != 1 || !nonterminalSet.count(startWords[0])) {
        cerr << "The start symbol must be one of the listed nonterminals.\n";
        return 1;
    }
    const string start = startWords[0];

    vector<Rule> rules;
    cout << "Enter one production line for each nonterminal, in the order listed.\n";
    cout << "Use the form A -> symbol symbol | epsilon:\n";
    for (const string& expectedLhs : nonterminals) {
        if (!getline(cin, line)) {
            cerr << "Missing production line.\n";
            return 1;
        }

        const size_t arrow = line.find("->");
        if (arrow == string::npos || line.find("->", arrow + 2) != string::npos) {
            cerr << "Each production line must contain exactly one '->'.\n";
            return 1;
        }

        const vector<string> lhsWords = splitWords(line.substr(0, arrow));
        if (lhsWords.size() != 1 || lhsWords[0] != expectedLhs) {
            cerr << "Expected a production for " << expectedLhs
                 << " in the listed nonterminal order.\n";
            return 1;
        }
        const string lhs = lhsWords[0];
        string alternativesText = line.substr(arrow + 2);
        string alternative;
        istringstream alternatives(alternativesText);
        while (getline(alternatives, alternative, '|')) {
            Production rhs = splitWords(alternative);
            if (rhs.empty() || (rhs.size() == 1 && rhs[0] == EPSILON)) {
                rhs.clear();
            } else if (find(rhs.begin(), rhs.end(), EPSILON) != rhs.end()) {
                cerr << "'epsilon' must appear alone as a production alternative.\n";
                return 1;
            }

            for (const string& symbol : rhs) {
                if (!terminals.count(symbol) && !nonterminalSet.count(symbol)) {
                    cerr << "Undeclared symbol in production: " << symbol << "\n";
                    return 1;
                }
            }
            rules.push_back({lhs, rhs});
        }
    }

    for (const string& nonterminal : nonterminals) {
        const bool hasRule = any_of(rules.begin(), rules.end(),
            [&](const Rule& rule) { return rule.first == nonterminal; });
        if (!hasRule) {
            cerr << "No production was provided for nonterminal " << nonterminal << ".\n";
            return 1;
        }
    }

    const map<string, SymbolSet> first = computeFirst(nonterminals, terminals, rules);
    const map<string, SymbolSet> follow =
        computeFollow(nonterminals, terminals, rules, start, first);
    printSets("FIRST", nonterminals, first);
    printSets("FOLLOW", nonterminals, follow);

    const ParsingTable table = makeTable(rules, terminals, nonterminalSet, first, follow);

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

    bool isLL1 = true;
    cout << "\nParsing table conflicts:\n";
    for (const auto& entry : table) {
        if (entry.second.size() > 1) {
            isLL1 = false;
            cout << "  M[" << entry.first.first << ", " << entry.first.second << "] has "
                 << entry.second.size() << " productions\n";
        }
    }
    if (isLL1) {
        cout << "No conflicts found.\n";
    }
    if (!isLL1) {
        cout << "The grammar is not LL(1); no string will be parsed.\n";
        return 0;
    }

    cout << "The grammar is LL(1).\n";
    cout << "Number of input strings to check: ";
    if (!getline(cin, line)) return 1;
    int inputCount = 0;
    try {
        size_t used = 0;
        inputCount = stoi(line, &used);
        if (used != line.size() || inputCount <= 0) {
            cerr << "Enter a positive whole number of input strings.\n";
            return 1;
        }
    } catch (const exception&) {
        cerr << "Enter a positive whole number of input strings.\n";
        return 1;
    }

    for (int i = 0; i < inputCount; ++i) {
        cout << "Input string " << (i + 1)
             << " (space-separated terminals; blank for epsilon): ";
        if (!getline(cin, line)) {
            cerr << "Missing input string.\n";
            return 1;
        }

        vector<string> tokens = splitWords(line);
        bool validTokens = true;
        for (const string& token : tokens) {
            if (!terminals.count(token)) {
                cout << "Rejected: '" << token << "' is not a listed terminal.\n";
                validTokens = false;
                break;
            }
        }

        bool accepted = false;
        if (validTokens) {
            tokens.push_back(END_MARKER);
            accepted = parseInput(tokens, start, terminals, nonterminalSet, table);
        }
        cout << "String " << (i + 1) << " is "
             << (accepted ? "accepted." : "not accepted.") << "\n\n";
    }
    return 0;
}
