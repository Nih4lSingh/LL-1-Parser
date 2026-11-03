# LL-1-Parser

## What is an LL(1) parser?

An LL(1) parser is a **top-down parser** that reads tokens from left to right and builds a parse by expanding grammar rules. The name describes how it works:

- **First L**: read the input from **left to right**.
- **Second L**: produce a **leftmost derivation** (expand the leftmost nonterminal first).
- **1**: use **one lookahead token** to decide which grammar rule to apply.

A *lookahead token* is the next input token the parser checks without consuming it yet.

## How does it work?

An LL(1) parser commonly uses a parsing table built from the grammar's **FIRST** and **FOLLOW** sets:

1. **Prepare the grammar.** Choose a start symbol and write the production rules. The grammar must be suitable for LL(1) parsing. In practice, this usually means removing left recursion and applying left factoring where needed.
2. **Build FIRST and FOLLOW sets.** FIRST tells which terminals can begin strings derived from a grammar symbol. FOLLOW tells which terminals can appear after a nonterminal.
3. **Fill the parsing table.** For each nonterminal and lookahead token, the table identifies the production to use.
4. **Parse the input.** Start with the start symbol on a stack. Compare the stack's top with the next input token:
   - If the top is a nonterminal, use the table entry to replace it with the right-hand side of a production.
   - If the top is a terminal matching the next token, consume that token and pop the stack.
   - If both the stack and input reach the end marker, the input is accepted. A missing table entry or a mismatch means the input is rejected.

### Small example

For the grammar:

```text
S → a A
A → b
```

the input `ab` is accepted: the parser expands `S` to `a A`, matches `a`, expands `A` to `b`, and then matches `b`.

## Why use an LL(1) parser?

LL(1) parsers are straightforward to implement and can parse input efficiently in a single pass. They are useful for learning how grammars, parsing tables, and syntax analysis fit together. Their main limitation is that not every grammar can be parsed as LL(1) without being rewritten.
