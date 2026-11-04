# LL(1)-Parser

A C++ program that reads a context-free grammar, computes FIRST and FOLLOW sets, checks whether the grammar is LL(1), and parses a token sequence when it is.

## Build and run

You need a C++17 compiler such as g++.

On Windows (PowerShell):

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o ll1_parser.exe
.\ll1_parser.exe
```

On Linux or macOS:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o ll1_parser
./ll1_parser
```

## Entering a grammar

Enter the terminals and nonterminals as space-separated symbols, then enter the start symbol. After that, enter **one production line for each nonterminal, in the same order as the nonterminal list**. The program uses the list to know how many production lines to read, so there is no separate production-count prompt.

Use `->` between the left and right sides, `|` between alternatives, and `epsilon` for an empty production. For example:

```text
Terminals: id + *
Nonterminals: E Q T R F
Start symbol: E
E -> T Q
Q -> + T Q | epsilon
T -> F R
R -> * F R | epsilon
F -> id
```

In the running program, enter each value when prompted. The production lines must appear in the listed order: first `E`, then `Q`, `T`, `R`, and `F`.

The program calculates and displays `FIRST` and `FOLLOW` for every nonterminal, then displays the filled entries in the generated parsing table. Each entry `M[A, a] = production` tells the parser which production to use when the stack has nonterminal `A` and the next input token is `a`. If a cell has multiple productions, they are shown separated by `|` and reported as a conflict, so the grammar is not LL(1). If there are no conflicts, the program says so and asks how many strings to check.

## Entering strings

After the grammar is confirmed as LL(1), enter how many strings you want to check. Then enter each string on its own line, using **space-separated terminals** from your grammar. The program reports whether each string is accepted.

For example, if you enter `2` as the number of strings, you can then enter:

```text
id + id * id
id + *
```

For every parsing step, the parser displays the stack and remaining input buffer before showing the action. After it matches a terminal, the next step shows the stack with that terminal popped and the input buffer with that token consumed. It then reports whether the string is accepted. Press Enter on a blank line to check the empty string.

## Input rules and current limits

- Every symbol must be listed as a terminal or nonterminal. The two lists must not overlap.
- Use one token per symbol, separated by spaces in the symbol lists, productions, and input string. A multi-character terminal such as `id` is fine.
- `epsilon` is reserved to mean an empty production; write it by itself as an alternative.
- The end marker `$` is added by the program. Do not list it as a terminal.
- The program detects whether the generated table has conflicts. It does not automatically rewrite a grammar that is not LL(1).
