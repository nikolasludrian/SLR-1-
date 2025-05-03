#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <queue>
#include <stack>
#include "first_follow.h"
#include "lexer.h"
#include "grammar.h"
#include "parser.h"
#include "utils.h"

std::vector<DFATransition> dfaEdges;
int main() {
	std::string filename = "grammar.txt";
	std::vector<Production> productions;
	std::set<std::string> terminals, nonTerminals;
	productions = readGrammarFromFile(filename, terminals, nonTerminals);
	if (productions.empty()) {
		std::cerr << "文法读取失败！" << std::endl;
		return 1;
	}
	// 在 readGrammarFromFile 后添加
	std::string startSymbol = productions[0].left;
	Production augmented = {"S'", {startSymbol}};
	productions.insert(productions.begin(), augmented);
	nonTerminals.insert("S'");
	nonTerminals.insert(startSymbol);
	// 打印读取的文法
	std::cout << "读取的文法：" << std::endl;
	for (const auto& prod : productions) {
		std::cout << prod.left << " -> ";
		for (const auto& sym : prod.right) {
			std::cout << sym << " ";
		}
		std::cout << std::endl;
	}
	
	
	// 计算 FIRST/FOLLOW 集
	auto firstSets = computeFirstSets(productions);
	auto followSets = computeFollowSets(productions, firstSets);
	printFirstSets(firstSets);
	printFollowSets(followSets);
	std::cout<<"===================项目集规范族（DFA的各状态）======================"<<std::endl;
	// 构造项目集规范族 DFA
	std::vector<DFATransition> dfaEdges;
	std::vector<std::string> orderedNonTerminals, orderedTerminals;
	extractOrderedSymbolsFromProductions(productions, orderedNonTerminals, orderedTerminals);
	
	auto states = buildCanonicalCollection(productions, orderedNonTerminals, orderedTerminals, dfaEdges);

	
	// 打印项目集
	for (int i = 0; i < states.size(); ++i) {
		printItemSet(states[i], productions, i);
	}
	
	// 打印 DFA 边
	printDFAEdges(dfaEdges);
	
	// 构造 ACTION / GOTO 表
	ActionTable actionTable;
	GotoTable gotoTable;
	std::map<ItemSet, int> stateIdMap;
	for (int i = 0; i < states.size(); ++i) {
		stateIdMap[states[i]] = i;
	}
	
	constructParsingTable(states, productions, dfaEdges, stateIdMap, followSets, 
		actionTable, gotoTable, orderedTerminals, orderedNonTerminals);
	 //打印分析表
	printParsingTable(actionTable, gotoTable);
// 从 test_code.txt 读取源代码
	std::ifstream codeFile("test_code.txt");
	if (!codeFile) {
		std::cerr << "无法打开 test_code.txt 文件！" << std::endl;
		return 1;
	}
	std::stringstream buffer;
	buffer << codeFile.rdbuf();
	std::string sourceCode = buffer.str();
	
	std::cout << "\n读取的源代码：" << std::endl;
	std::cout << sourceCode << std::endl;
	
// 调用词法分析器
	std::vector<Token> tokens = tokenize(sourceCode);
	
// 打印词法分析结果
	std::cout << "\n词法分析结果（Token 列表）：" << std::endl;
	printTokens(tokens);
	std::cout << std::endl;
	
// 将 Token 转换为语法分析器所需的字符串（如：id + num * id ;）
	// 将 Token 转换为语法分析器所需的字符串（如：id + num * id）
	string tokenStream;
	for (const auto& token : tokens) {
		if (token.type == TokenType::ID) {
			tokenStream += "id ";
		}
		else if (token.type == TokenType::NUM) {
			tokenStream += "num ";
		}
		else if (token.type == TokenType::SYM || token.type == TokenType::KEYWORD) {
			tokenStream += token.lexeme + " ";
		}
		else if (token.type == TokenType::END_OF_FILE) {
			// 可以选择加上 "$" 结束符，也可以不加，看你的语法分析器需求
			tokenStream += "$";
		}
		else {
			cerr << "未知 Token 类型" << endl;
			exit(1);
		}
	}
	
	std::cout << "\n转换后的 Token 串（用于语法分析）：" << std::endl;
	std::cout << tokenStream << std::endl;
	
// 调用语法分析器
	std::shared_ptr<TreeNode> syntaxTree = analyzeInputString(tokenStream, productions, actionTable, gotoTable);
}

