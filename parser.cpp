#include "parser.h"
#include <queue>
#include <stack>
#include <iostream>
#include <sstream>
#include <algorithm>

ItemSet closure(const ItemSet& itemSet, const std::vector<Production>& productions) {
	ItemSet closureSet = itemSet;
	bool added;
	
	do {
		added = false;
		std::set<Item> newItems;
		
		for (const auto& item : closureSet.items) {
			const Production& prod = productions[item.productionIndex];
			
			if (item.dotPosition < prod.right.size()) {
				std::string symbolAfterDot = prod.right[item.dotPosition];
				
				bool isNonTerminal = false;
				for (const auto& p : productions) {
					if (p.left == symbolAfterDot) {
						isNonTerminal = true;
						break;
					}
				}
				if (!isNonTerminal) continue;
				
				for (int i = 0; i < productions.size(); ++i) {
					if (productions[i].left == symbolAfterDot) {
						Item newItem = {i, 0};
						if (closureSet.items.find(newItem) == closureSet.items.end()) {
							newItems.insert(newItem);
							added = true;
						}
					}
				}
			}
		}
		
		closureSet.items.insert(newItems.begin(), newItems.end());
		
	} while (added);
	
	return closureSet;
}

ItemSet GOTO(const ItemSet& itemSet, const std::string& symbol, const std::vector<Production>& productions) {
	ItemSet gotoSet;
	
	for (const auto& item : itemSet.items) {
		const Production& prod = productions[item.productionIndex];
		
		if (item.dotPosition < prod.right.size() && prod.right[item.dotPosition] == symbol) {
			Item movedItem = {item.productionIndex, item.dotPosition + 1};
			gotoSet.items.insert(movedItem);
		}
	}
	
	return closure(gotoSet, productions);
}

std::vector<ItemSet> buildCanonicalCollection(
	const std::vector<Production>& productions,
	const std::vector<std::string>& orderedNonTerminals,
	const std::vector<std::string>& orderedTerminals,
	std::vector<DFATransition>& dfaEdges)
{
	std::vector<ItemSet> states;
	std::map<ItemSet, int> stateIdMap;
	
	// 初始项目集：S' -> .S
	ItemSet startSet;
	startSet.items.insert({0, 0}); // 第0条产生式，点在0位置
	for (int i = 1; i < productions.size(); ++i) {
		startSet.items.insert({i, 0}); // 每条产生式，点在最左边
	}
	startSet = closure(startSet, productions);
	
	states.push_back(startSet);
	stateIdMap[startSet] = 0;
	
	std::queue<ItemSet> q;
	q.push(startSet);
	
	while (!q.empty()) {
		ItemSet current = q.front();
		q.pop();
		int fromId = stateIdMap[current];
		
		//先遍历非终结符
		for (const auto& symbol : orderedNonTerminals) {
			ItemSet next = GOTO(current, symbol, productions);
			if (!next.items.empty()) {
				int toId;
				if (stateIdMap.count(next) == 0) {
					toId = states.size();
					stateIdMap[next] = toId;
					states.push_back(next);
					q.push(next);
				} else {
					toId = stateIdMap[next];
				}
				dfaEdges.push_back({fromId, symbol, toId});
			}
		}
		
		//  再遍历终结符
		for (const auto& symbol : orderedTerminals) {
			ItemSet next = GOTO(current, symbol, productions);
			if (!next.items.empty()) {
				int toId;
				if (stateIdMap.count(next) == 0) {
					toId = states.size();
					stateIdMap[next] = toId;
					states.push_back(next);
					q.push(next);
				} else {
					toId = stateIdMap[next];
				}
				dfaEdges.push_back({fromId, symbol, toId});
			}
		}
	}
	
	return states;
}

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
	) {
		std::set<std::string> terminals(orderedTerminals.begin(), orderedTerminals.end());
		std::set<std::string> nonTerminals(orderedNonTerminals.begin(), orderedNonTerminals.end());
		
		// 打印一下 terminals 和 nonTerminals
		std::cout << "Terminals: ";
		int cnt = 0;
		for (auto& t : orderedTerminals) {
			std::cout << t << " ";
			if (++cnt >= 50) { std::cout << "...(too many)" << std::endl; break; }
		}
		std::cout << std::endl;
		
		std::cout << "NonTerminals: ";
		cnt = 0;
		for (auto& nt : orderedNonTerminals) {
			std::cout << nt << " ";
			if (++cnt >= 50) { std::cout << "...(too many)" << std::endl; break; }
		}
		std::cout << std::endl;
		
		// 处理 DFA 转移
		for (const auto& edge : dfaEdges) {
			if (terminals.count(edge.symbol)) {
				actionTable[{edge.from, edge.symbol}] = {SHIFT, edge.to};
			} else if (nonTerminals.count(edge.symbol)) {
				gotoTable[{edge.from, edge.symbol}] = edge.to;
			}
		}
		
		// 处理规约/接受动作
		for (int i = 0; i < states.size(); ++i) {
			for (const auto& item : states[i].items) {
				const Production& prod = productions[item.productionIndex];
				if (item.dotPosition == prod.right.size()) {
					if (prod.left == "S'") {
						actionTable[{i, "$"}] = {ACCEPT, -1};
					} else {
						for (const auto& a : followSets.at(prod.left)) {
							actionTable[{i, a}] = {REDUCE, item.productionIndex};
						}
					}
				}
			}
		}
	}
// 辅助函数：递归遍历语法树，输出 DOT 格式内容
void writeDot(std::ofstream& out, std::shared_ptr<TreeNode> node, int& id, int parentId ) {
	if (!node) return;
	
	int currentId = id++;
	out << "    node" << currentId << " [label=\"" << node->value << "\"];" << std::endl;
	
	if (parentId != -1) {
		out << "    node" << parentId << " -> node" << currentId << ";" << std::endl;
	}
	
	for (auto& child : node->children) {
		writeDot(out, child, id, currentId);
	}
}

// 生成 DOT 文件
void generateDotFile(std::shared_ptr<TreeNode> root) {
	std::ofstream outFile("syntax_tree.dot");
	if (!outFile.is_open()) {
		std::cerr << "无法打开文件 syntax_tree.dot 进行写入！" << std::endl;
		return;
	}
	
	outFile << "digraph SyntaxTree {" << std::endl;
	outFile << "    node [shape=ellipse];" << std::endl;
	
	int id = 0;
	writeDot(outFile, root, id);
	
	outFile << "}" << std::endl;
	outFile.close();
}
std::shared_ptr<TreeNode> analyzeInputString(
	const std::string& input,
	const std::vector<Production>& productions,
	const ActionTable& actionTable,
	const GotoTable& gotoTable) {
		
		std::stack<int> stateStack;
		std::stack<std::string> symbolStack;
		
		std::vector<std::string> inputSymbols;
		std::istringstream iss(input);
		std::string token;
		while (iss >> token) {
			inputSymbols.push_back(token);
		}
		inputSymbols.push_back("$"); // 结束符
		
		stateStack.push(0); // 初始状态
		int ip = 0;         // 输入指针
		int step = 0;       // 步骤编号
		
		// 语法树的根节点
		std::shared_ptr<TreeNode> syntaxTreeRoot = nullptr;
		std::shared_ptr<TreeNode> currentNode = nullptr;
		
		std::cout << "\n================= 分析过程 =================\n";
		std::cout << std::left
		<< std::setw(8) << "步骤"
		<< std::setw(25) << "状态栈"
		<< std::setw(25) << "符号栈"
		<< std::setw(50) << "剩余输入"
		<< std::setw(30) << "动作" << "\n";
		std::cout << std::string(138, '-') << "\n";
		
		while (true) {
			++step;
			int currentState = stateStack.top();
			std::string currentSymbol = inputSymbols[ip];
			
			// 查找 ACTION
			auto actionIt = actionTable.find({currentState, currentSymbol});
			if (actionIt == actionTable.end()) {
				std::cout << step << "\t错误：无效动作 (state = " << currentState
				<< ", symbol = '" << currentSymbol << "')\n";
				break;
			}
			
			Action action = actionIt->second;
			std::string actionStr;
			
			if (action.type == SHIFT) {
				// Shift 动作
				stateStack.push(action.target);
				symbolStack.push(currentSymbol);
				++ip;
				actionStr = "shift " + std::to_string(action.target);
			} 
			else if (action.type == REDUCE) {
				const Production& prod = productions[action.target];
				int popCount = prod.right.size();
				for (int i = 0; i < popCount; ++i) {
					if (!symbolStack.empty()) symbolStack.pop();
					if (!stateStack.empty()) stateStack.pop();
				}
				symbolStack.push(prod.left);
				
				int topState = stateStack.top();
				auto gotoIt = gotoTable.find({topState, prod.left});
				if (gotoIt == gotoTable.end()) {
					std::cout << step << "\t错误：无效的 GOTO (state = " << topState 
					<< ", non-terminal = '" << prod.left << "')\n";
					break;
				}
				int gotoState = gotoIt->second;
				stateStack.push(gotoState);
				
				actionStr = "reduce " + prod.left + " → ";
				for (const auto& s : prod.right) {
					actionStr += s + " ";
				}
				
				// 在 REDUCE 动作时构建语法树
				std::shared_ptr<TreeNode> reduceNode = std::make_shared<TreeNode>(prod.left);
				
				// 为产生式右侧的每个符号创建子节点
				for (auto& rightSymbol : prod.right) {
					std::shared_ptr<TreeNode> childNode = std::make_shared<TreeNode>(rightSymbol);
					reduceNode->children.push_back(childNode);
				}
				
				// 如果是 S 的产生式，则设置为语法树的根节点
				if (prod.left == "S") {
					syntaxTreeRoot = reduceNode; // 直接设置根节点
				} else if (currentNode) {
					// 否则，将当前节点的子节点指向该节点
					currentNode->children.push_back(reduceNode);
				}
				
				currentNode = reduceNode;  // 更新当前节点为新创建的节点
			}
			else if (action.type == ACCEPT) {
				actionStr = "accept";
			}
			
			// 打印当前步骤的状态
			std::ostringstream statesStream, symbolsStream, inputStream;
			
			// 状态栈
			{
				std::stack<int> temp = stateStack;
				std::vector<int> states;
				while (!temp.empty()) {
					states.push_back(temp.top());
					temp.pop();
				}
				std::reverse(states.begin(), states.end());
				for (auto s : states) statesStream << s << " ";
			}
			
			// 符号栈
			{
				std::stack<std::string> temp = symbolStack;
				std::vector<std::string> symbols;
				while (!temp.empty()) {
					symbols.push_back(temp.top());
					temp.pop();
				}
				std::reverse(symbols.begin(), symbols.end());
				for (const auto& s : symbols) symbolsStream << s << " ";
			}
			
			// 剩余输入
			for (int i = ip; i < inputSymbols.size(); ++i) {
				inputStream << inputSymbols[i] << " ";
			}
			
			// 打印当前步骤
			std::cout << std::left
			<< std::setw(8) << step
			<< std::setw(25) << statesStream.str()
			<< std::setw(25) << symbolsStream.str()
			<< std::setw(50) << inputStream.str()
			<< std::setw(30) << actionStr
			<< "\n";
			
			if (action.type == ACCEPT) {
				std::cout << "\n 输入串被成功接受！" << std::endl;
				break;
			}
		}
		
		// 生成语法树的可视化
		generateDotFile(syntaxTreeRoot);
		std::cout << "语法树已生成，文件名为 syntax_tree.dot" << std::endl;
		
		// 返回语法树的根节点
		return syntaxTreeRoot;
	}

