#pragma once

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

constexpr char EPSILON[] = "epsilon";
constexpr char END_MARKER[] = "$";

using Production = std::vector<std::string>;
using Rule = std::pair<std::string, Production>;
using SymbolSet = std::set<std::string>;
using ParsingTable = std::map<std::pair<std::string, std::string>,
                               std::vector<Production>>;
