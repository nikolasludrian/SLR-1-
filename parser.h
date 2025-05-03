#pragma once
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <iomanip> // 加这个头文件,优化排版
#include <memory>
#include "token.h"
#include "first_follow.h"

enum ActionType { SHIFT, REDUCE, ACCEPT, ERROR };

struct Action {
	ActionType type;
	int target;
};

struct DFATransition { int from; std::string symbol; int to; };

struct Item {
	int productionIndex; // index in the production list
	int dotPosition;     // position of dot
	
	bool operator==(const Item& other) const {
		return productionIndex == other.productionIndex &&
		dotPosition == other.dotPosition;
	}
	bool operator<(const Item& other) const {
		return std::tie(productionIndex, dotPosition) < std::tie(other.productionIndex, other.dotPosition);
	}
};
struct ItemSet {
	std::set<Item> items;
	
	bool operator==(const ItemSet& other) const {
		return items == other.items;
	}
	bool operator<(const ItemSet& other) const {
		return items < other.items; // 利用 std::set 的比较
	}
};
struct TreeNode {
	std::string value; // 节点的值（类型或符号名）
	std::vector<std::shared_ptr<TreeNode>> children; // 子节点
	
	TreeNode(const std::string& val) : value(val) {}
};

using ActionTable = std::map<std::pair<int, std::string>, Action>;
using GotoTable = std::map<std::pair<int, std::string>, int>;

ItemSet closure(const ItemSet& itemSet, const std::vector<Production>& productions);
ItemSet GOTO(const ItemSet& itemSet, const std::string& symbol, const std::vector<Production>& productions);
std::vector<ItemSet> buildCanonicalCollection(
	const std::vector<Production>& productions,
	const std::vector<std::string>& orderedNonTerminals,
	const std::vector<std::string>& orderedTerminals,
	std::vector<DFATransition>& dfaEdges );  // 新增参数用于输出边; 
void constructParsingTable(
	const std::vector<ItemSet>& states,
	const std::vector<Production>& productions,
	const std::vector<DFATransition>& dfaEdges,
	const std::map<ItemSet, int>& stateIdMap,
	const std::map<std::string, std::set<std::string>>& followSets,
	ActionTable& actionTable,
	GotoTable& gotoTable,
	const std::vector<std::string>& orderedTerminals,   // 用 vector
	const std::vector<std::string>& orderedNonTerminals // 用 vector
	);
void writeDot(std::ofstream& out, std::shared_ptr<TreeNode> node, int& id, int parentId = -1);
void generateDotFile(std::shared_ptr<TreeNode> root);
std::shared_ptr<TreeNode> analyzeInputString(const std::string& input,
	const std::vector<Production>& productions,
	const ActionTable& actionTable,
	const GotoTable& gotoTable);

