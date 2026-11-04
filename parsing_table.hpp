#pragma once

#include "grammar.hpp"

#include <string>
#include <vector>

ParsingTable makeTable(
    const std::vector<Rule>& rules,
    const SymbolSet& terminals,
    const std::map<std::string, SymbolSet>& first,
    const std::map<std::string, SymbolSet>& follow);

std::string productionText(const Production& production);

void printParsingTable(const std::vector<std::string>& nonterminals,
                       const SymbolSet& terminals,
                       const ParsingTable& table);

// Prints conflicts, or "No conflicts found." Returns true when the table is conflict-free.
bool reportConflicts(const ParsingTable& table);
