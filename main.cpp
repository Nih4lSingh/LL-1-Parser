#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

using Production = vector<string>;
using Table = map<pair<string, string>, Production>;

// Tokenize identifiers and numbers as "id", and recognize +, *, (, and ).
bool tokenize(const string& input, vector<string>& tokens, string& error) {
    for (size_t i = 0; i < input.size();) {
        const unsigned char ch = static_cast<unsigned char>(input[i]);

        if (isspace(ch)) {
            ++i;
        } else if (isalpha(ch) || ch == '_') {
            while (i < input.size()) {
                const unsigned char next = static_cast<unsigned char>(input[i]);
                if (!isalnum(next) && next != '_') break;
                ++i;
            }
            tokens.push_back("id");
        } else if (isdigit(ch)) {
            while (i < input.size() &&
                   isdigit(static_cast<unsigned char>(input[i]))) {
                ++i;
            }
            tokens.push_back("id");
        } else if (input[i] == '+' || input[i] == '*' ||
                   input[i] == '(' || input[i] == ')') {
            tokens.emplace_back(1, input[i]);
            ++i;
        } else {
            error = string("Unrecognized character: '") + input[i] + "'";
            return false;
        }
    }

    tokens.push_back("$"); // End-of-input marker.
    return true;
}

Table makeParsingTable() {
    // Grammar:
    // E  -> T Q
    // Q  -> + T Q | epsilon
    // T  -> F R
    // R  -> * F R | epsilon
    // F  -> ( E ) | id
    Table table;
    table[{"E", "id"}] = {"T", "Q"};
    table[{"E", "("}] = {"T", "Q"};

    table[{"Q", "+"}] = {"+", "T", "Q"};
    table[{"Q", ")"}] = {}; // epsilon
    table[{"Q", "$"}] = {}; // epsilon

    table[{"T", "id"}] = {"F", "R"};
    table[{"T", "("}] = {"F", "R"};

    table[{"R", "*"}] = {"*", "F", "R"};
    table[{"R", "+"}] = {}; // epsilon
    table[{"R", ")"}] = {}; // epsilon
    table[{"R", "$"}] = {}; // epsilon

    table[{"F", "("}] = {"(", "E", ")"};
    table[{"F", "id"}] = {"id"};
    return table;
}

bool isNonterminal(const string& symbol) {
    return symbol == "E" || symbol == "Q" ||
           symbol == "T" || symbol == "R" || symbol == "F";
}

bool parse(const vector<string>& tokens, const Table& table) {
    vector<string> stack = {"$", "E"};
    size_t inputPosition = 0;

    cout << "\nParsing steps:\n";
    cout << "Stack (top at right) | Next token | Action\n";

    while (!stack.empty()) {
        const string top = stack.back();
        const string lookahead = tokens[inputPosition];

        string stackView;
        for (const string& symbol : stack) stackView += symbol + " ";
        cout << stackView << "| " << lookahead << " | ";

        if (top == "$" && lookahead == "$") {
            cout << "accept\n";
            return true;
        }

        if (!isNonterminal(top)) {
            if (top != lookahead) {
                cout << "error: expected '" << top << "'\n";
                return false;
            }
            cout << "match '" << lookahead << "'\n";
            stack.pop_back();
            ++inputPosition;
            continue;
        }

        const auto entry = table.find({top, lookahead});
        if (entry == table.end()) {
            cout << "error: no table entry for (" << top
                 << ", " << lookahead << ")\n";
            return false;
        }

        cout << "expand " << top << " -> ";
        if (entry->second.empty()) {
            cout << "epsilon\n";
        } else {
            for (const string& symbol : entry->second) cout << symbol;
            cout << '\n';
        }

        stack.pop_back();
        // Push the production in reverse so its first symbol is processed next.
        for (auto it = entry->second.rbegin(); it != entry->second.rend(); ++it) {
            stack.push_back(*it);
        }
    }

    return false;
}

int main() {
    cout << "LL(1) arithmetic expression parser\n";
    cout << "Grammar: E -> TQ, Q -> +TQ | epsilon, T -> FR, "
            "R -> *FR | epsilon, F -> (E) | id\n";
    cout << "Enter an expression (identifiers/numbers, +, *, parentheses): ";

    string input;
    if (!getline(cin, input)) {
        cerr << "Could not read input.\n";
        return 1;
    }

    vector<string> tokens;
    string error;
    if (!tokenize(input, tokens, error)) {
        cout << "Rejected: " << error << '\n';
        return 1;
    }

    const bool accepted = parse(tokens, makeParsingTable());
    cout << (accepted ? "\nAccepted: the expression matches the grammar.\n"
                      : "\nRejected: the expression does not match the grammar.\n");
    return accepted ? 0 : 1;
}
