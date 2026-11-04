#pragma once

#include "grammar.hpp"

#include <string>
#include <vector>

bool parseInput(const std::vector<std::string>& tokens,
                const std::string& start,
                const SymbolSet& terminals,
                const SymbolSet& nonterminals,
                const ParsingTable& table);
