#pragma once
#include <vector>
#include <set>
#include <string>
#include "token.h"
#include "first_follow.h"
std::vector<Production> readGrammarFromFile(
	const std::string& filename,
	std::set<std::string>& terminals,
	std::set<std::string>& nonTerminals);

