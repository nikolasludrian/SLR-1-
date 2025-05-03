#include "first_follow.h"
#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

// 判断是否是终结符（非大写字母为终结符）
bool isTerminal(const string& symbol) {
	return !symbol.empty() && !isupper(symbol[0]) && symbol != "eta";
}

// 计算某个特定符号串的first，用于 FOLLOW 集计算
set<string> computeFirst(const vector<string>& symbols,
	const map<string, set<string>>& firstSets) {
		set<string> result;
		
		for (const string& sym : symbols) {
			if (isTerminal(sym)) {
				result.insert(sym);
				return result;
			}
			
			const auto& firstSym = firstSets.at(sym);
			for (const auto& f : firstSym) {
				if (f != "eta")
					result.insert(f);
			}
			
			if (firstSym.count("eta") == 0) {
				return result;
			}
		}
		
		// 所有符号都能推出 eta
		result.insert("eta");
		return result;
	}

// 计算 FIRST 集合
map<string, set<string>> computeFirstSets(const vector<Production>& productions) {
	map<string, vector<vector<string>>> grammar;
	map<string, set<string>> firstSet;
	
	// 构建 grammar 映射
	for (const auto& prod : productions) {
		grammar[prod.left].push_back(prod.right);
	}
	
	// 初始化终结符
	for (const auto& [lhs, prods] : grammar) {
		for (const auto& rhs : prods) {
			for (const auto& sym : rhs) {
				if (isTerminal(sym)) {
					firstSet[sym] = {sym};
				}
			}
		}
	}
	
	// 初始化非终结符
	for (const auto& [lhs, _] : grammar) {
		firstSet[lhs]; // 确保存在
	}
	
	bool updated;
	do {
		updated = false;
		for (const auto& [lhs, prods] : grammar) {
			for (const auto& prod : prods) {
				auto f = computeFirst(prod, firstSet);
				size_t before = firstSet[lhs].size();
				firstSet[lhs].insert(f.begin(), f.end());
				if (firstSet[lhs].size() > before) {
					updated = true;
				}
			}
		}
	} while (updated);
	
	return firstSet;
}

// 计算 FOLLOW 集合
map<string, set<string>> computeFollowSets(
	const vector<Production>& productions,
	const map<string, set<string>>& firstSet
	) {
		map<string, vector<vector<string>>> grammar;
		map<string, set<string>> followSet;
		
		// 构建 grammar 映射
		for (const auto& prod : productions) {
			grammar[prod.left].push_back(prod.right);
		}
		
		// 初始化 followSet
		for (const auto& [lhs, _] : grammar) {
			followSet[lhs]; // 确保存在
		}
		
		// 起始符号加入 $
		if (!productions.empty()) {
			followSet[productions[0].left].insert("$");
		}
		
		bool updated;
		do {
			updated = false;
			for (const auto& [lhs, prods] : grammar) {
				for (const auto& prod : prods) {
					for (size_t i = 0; i < prod.size(); ++i) {
						const string& B = prod[i];
						if (isTerminal(B)) continue;
						
						vector<string> beta(prod.begin() + i + 1, prod.end());
						set<string> followB_before = followSet[B];
						
						if (!beta.empty()) {
							set<string> firstBeta = computeFirst(beta, firstSet);
							
							for (const string& symbol : firstBeta) {
								if (symbol != "eta")
									followSet[B].insert(symbol);
							}
							
							if (firstBeta.count("eta")) {
								followSet[B].insert(followSet[lhs].begin(), followSet[lhs].end());
							}
						} else {
							followSet[B].insert(followSet[lhs].begin(), followSet[lhs].end());
						}
						
						if (followSet[B].size() > followB_before.size()) {
							updated = true;
						}
					}
				}
			}
		} while (updated);
		
		return followSet;
	}

// 打印 FIRST/FOLLOW 集
void printSet(const map<string, set<string>>& sets, const string& name) {
	cout << "==== " << name << " Sets ====" << endl;
	for (const auto& [symbol, sset] : sets) {
		cout << name << "(" << symbol << ") = { ";
		for (const auto& s : sset) {
			cout << s << " ";
		}
		cout << "}" << endl;
	}
}
//int main() {
//	std::vector<Production> productions = {
//		{"S'", {"S"}},
//		{"S", {"A"}},
//		{"A", {"a", "A"}},
//		{"A", {"b"}}
//	};
//	
//	auto firstSets = computeFirstSets(productions);
//	auto followSets = computeFollowSets(productions, firstSets);
//	
//	printSet(firstSets, "FIRST");
//	printSet(followSets, "FOLLOW");
//	
//	return 0;
//}
