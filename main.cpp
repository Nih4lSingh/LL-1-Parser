#include "first_follow.hpp"
#include "parser.hpp"
#include "parsing_table.hpp"

#include <bits/stdc++.h>
using namespace std;

vector<string> splitWords(const string& line) {
    istringstream input(line);
    vector<string> words;
    string word;
    while (input >> word) words.push_back(word);
    return words;
}

int main() {
    cout << "LL(1) Grammar Analyzer and Parser\\n";
    cout << "Enter symbols separated by spaces. Use 'epsilon' for an empty "
            "production; do not include $ in the terminal list.\\n";

    string line;
    cout << "\\nTerminals: ";
    if (!getline(cin, line)) return 1;
    const vector<string> terminalWords = splitWords(line);
    const SymbolSet terminals(terminalWords.begin(), terminalWords.end());
    if (terminals.empty() || terminals.count(EPSILON) || terminals.count(END_MARKER)) {
        cerr << "Invalid terminal list. It must be non-empty and cannot contain "
                "'epsilon' or '$'.\\n";
        return 1;
    }

    cout << "Nonterminals: ";
    if (!getline(cin, line)) return 1;
    const vector<string> nonterminals = splitWords(line);
    const SymbolSet nonterminalSet(nonterminals.begin(), nonterminals.end());
    if (nonterminals.empty()) {
        cerr << "The nonterminal list cannot be empty.\\n";
        return 1;
    }
    for (const string& symbol : nonterminalSet) {
        if (terminals.count(symbol) || symbol == EPSILON || symbol == END_MARKER) {
            cerr << "Symbol lists overlap or use a reserved symbol: " << symbol << "\\n";
            return 1;
        }
    }

    cout << "Start symbol: ";
    if (!getline(cin, line)) return 1;
    const vector<string> startWords = splitWords(line);
    if (startWords.size() != 1 || !nonterminalSet.count(startWords[0])) {
        cerr << "The start symbol must be one of the listed nonterminals.\\n";
        return 1;
    }
    const string start = startWords[0];

    vector<Rule> rules;
    cout << "Enter one production line for each nonterminal, in the order listed.\\n";
    cout << "Use the form A -> symbol symbol | epsilon:\\n";
    for (const string& expectedLhs : nonterminals) {
        if (!getline(cin, line)) {
            cerr << "Missing production line.\\n";
            return 1;
        }

        const size_t arrow = line.find("->");
        if (arrow == string::npos || line.find("->", arrow + 2) != string::npos) {
            cerr << "Each production line must contain exactly one '->'.\\n";
            return 1;
        }

        const vector<string> lhsWords = splitWords(line.substr(0, arrow));
        if (lhsWords.size() != 1 || lhsWords[0] != expectedLhs) {
            cerr << "Expected a production for " << expectedLhs
                 << " in the listed nonterminal order.\\n";
            return 1;
        }
        const string lhs = lhsWords[0];
        const string alternativesText = line.substr(arrow + 2);
        istringstream alternatives(alternativesText);
        string alternative;
        while (getline(alternatives, alternative, '|')) {
            Production rhs = splitWords(alternative);
            if (rhs.empty() || (rhs.size() == 1 && rhs[0] == EPSILON)) {
                rhs.clear();
            } else if (find(rhs.begin(), rhs.end(), EPSILON) != rhs.end()) {
                cerr << "'epsilon' must appear alone as a production alternative.\\n";
                return 1;
            }

            for (const string& symbol : rhs) {
                if (!terminals.count(symbol) && !nonterminalSet.count(symbol)) {
                    cerr << "Undeclared symbol in production: " << symbol << "\\n";
                    return 1;
                }
            }
            rules.push_back({lhs, rhs});
        }
    }

    const map<string, SymbolSet> first = computeFirst(nonterminals, terminals, rules);
    const map<string, SymbolSet> follow =
        computeFollow(nonterminals, terminals, rules, start, first);
    printSets("FIRST", nonterminals, first);
    printSets("FOLLOW", nonterminals, follow);

    const ParsingTable table = makeTable(rules, terminals, first, follow);
    printParsingTable(nonterminals, terminals, table);
    if (!reportConflicts(table)) {
        cout << "The grammar is not LL(1); no string will be parsed.\\n";
        return 0;
    }

    cout << "The grammar is LL(1).\\n";
    cout << "Number of input strings to check: ";
    if (!getline(cin, line)) return 1;
    int inputCount = 0;
    try {
        size_t used = 0;
        inputCount = stoi(line, &used);
        if (used != line.size() || inputCount <= 0) {
            cerr << "Enter a positive whole number of input strings.\\n";
            return 1;
        }
    } catch (const exception&) {
        cerr << "Enter a positive whole number of input strings.\\n";
        return 1;
    }

    for (int i = 0; i < inputCount; ++i) {
        cout << "Input string " << (i + 1)
             << " (space-separated terminals; blank for epsilon): ";
        if (!getline(cin, line)) {
            cerr << "Missing input string.\\n";
            return 1;
        }

        vector<string> tokens = splitWords(line);
        bool validTokens = true;
        for (const string& token : tokens) {
            if (!terminals.count(token)) {
                cout << "Rejected: '" << token << "' is not a listed terminal.\\n";
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
             << (accepted ? "accepted." : "not accepted.") << "\\n\\n";
    }
    return 0;
}
