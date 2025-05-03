#include "utils.h"
#include <iostream>
#include <algorithm>

void printItemSet(const ItemSet& itemSet, const std::vector<Production>& productions, int stateId) {
	std::cout << "State " << stateId << ":\n";
	for (const auto& item : itemSet.items) {
		const auto& prod = productions[item.productionIndex];
		std::cout << "  " << prod.left << " → ";
		for (int i = 0; i < prod.right.size(); ++i) {
			if (i == item.dotPosition) std::cout << ". ";
			std::cout << prod.right[i] << " ";
		}
		if (item.dotPosition == prod.right.size()) std::cout << ".";
		std::cout << "\n";
	}
	std::cout << std::endl;
}

void printDFAEdges(const std::vector<DFATransition>& edges) {
	std::cout << "------------------EDGE---------------------" << std::endl;
	for (const auto& edge : edges) {
		std::cout << "I" << edge.from << "--" << edge.symbol << "--I" << edge.to << std::endl;
	}
}

void printFirstSets(const std::map<std::string, std::set<std::string>>& firstSets) {
	std::cout << "\n===== FIRST 集 =====\n";
	for (const auto& entry : firstSets) {
		std::cout << "FIRST(" << entry.first << ") = { ";
		for (const auto& sym : entry.second) {
			std::cout << sym << " ";
		}
		std::cout << "}\n";
	}
}

void printFollowSets(const std::map<std::string, std::set<std::string>>& followSets) {
	std::cout << "\n===== FOLLOW 集 =====\n";
	for (const auto& entry : followSets) {
		std::cout << "FOLLOW(" << entry.first << ") = { ";
		for (const auto& sym : entry.second) {
			std::cout << sym << " ";
		}
		std::cout << "}\n";
	}
}
void extractOrderedSymbolsFromProductions(
	const std::vector<Production>& productions,
	std::vector<std::string>& orderedNonTerminals,
	std::vector<std::string>& orderedTerminals)
{
	std::set<std::string> seenNonTerminals;
	std::set<std::string> seenTerminals;
	
	for (const auto& prod : productions) {
		if (seenNonTerminals.insert(prod.left).second) {
			orderedNonTerminals.push_back(prod.left);
		}
		
		for (const auto& sym : prod.right) {
			if (isupper(sym[0])) { // 简单判断：大写开头为非终结符
				if (seenNonTerminals.insert(sym).second) {
					orderedNonTerminals.push_back(sym);
				}
			} else {
				if (seenTerminals.insert(sym).second) {
					orderedTerminals.push_back(sym);
				}
			}
		}
	}
}
void printParsingTable(const ActionTable& actionTable, const GotoTable& gotoTable) {
	std::cout << "ACTION Table:\n";
	for (const auto& entry : actionTable) {
		std::cout << "ACTION[ " << entry.first.first << ", " << entry.first.second << " ] = ";
		if (entry.second.type == SHIFT)
			std::cout << "S" << entry.second.target;
		else if (entry.second.type == REDUCE)
			std::cout << "r" << entry.second.target;
		else if (entry.second.type == ACCEPT)
			std::cout << "acc";
		std::cout << std::endl;
	}
	
	std::cout << "\nGOTO Table:\n";
	for (const auto& entry : gotoTable) {
		std::cout << "GOTO[ " << entry.first.first << ", " << entry.first.second << " ] = " << entry.second << std::endl;
	}
}

