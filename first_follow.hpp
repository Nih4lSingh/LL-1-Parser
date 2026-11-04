#pragma once

#include "grammar.hpp"

#include <map>
#include <string>
#include <vector>

SymbolSet firstOfSequence(const Production& sequence,
                          const SymbolSet& terminals,
                          const std::map<std::string, SymbolSet>& first);

std::map<std::string, SymbolSet> computeFirst(
    const std::vector<std::string>& nonterminals,
    const SymbolSet& terminals,
    const std::vector<Rule>& rules);

std::map<std::string, SymbolSet> computeFollow(
    const std::vector<std::string>& nonterminals,
    const SymbolSet& terminals,
    const std::vector<Rule>& rules,
    const std::string& start,
    const std::map<std::string, SymbolSet>& first);

void printSets(const std::string& title,
               const std::vector<std::string>& nonterminals,
               const std::map<std::string, SymbolSet>& sets);
