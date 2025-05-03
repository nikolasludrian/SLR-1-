#pragma once
#include <vector>
#include <map>
#include "token.h"
#include "parser.h"
void printItemSet(const ItemSet& itemSet, const std::vector<Production>& productions, int stateId);
void printDFAEdges(const std::vector<DFATransition>& edges);
void printFirstSets(const std::map<std::string, std::set<std::string>>& firstSets);
void printFollowSets(const std::map<std::string, std::set<std::string>>& followSets);
void extractOrderedSymbolsFromProductions(
	const std::vector<Production>& productions,
	std::vector<std::string>& orderedNonTerminals,
	std::vector<std::string>& orderedTerminals);
void printParsingTable(const ActionTable& actionTable, const GotoTable& gotoTable);
