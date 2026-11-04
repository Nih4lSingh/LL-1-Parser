#include "parser.hpp"

#include "parsing_table.hpp"

#include <iostream>

using namespace std;

bool parseInput(const vector<string>& tokens, const string& start,
                const SymbolSet& terminals, const SymbolSet& nonterminals,
                const ParsingTable& table) {
    vector<string> stack = {END_MARKER, start};
    size_t position = 0;

    cout << "\nParsing steps (stack top is on the left):\n";
    size_t step = 1;
    while (!stack.empty()) {
        const string top = stack.back();
        const string lookahead = tokens[position];

        cout << "Step " << step++ << ":\n";
        cout << "  Stack: ";
        for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
            cout << *it << " ";
        }
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
