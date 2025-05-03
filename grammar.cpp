#include "grammar.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

std::vector<Production> readGrammarFromFile(const std::string& filename,
	std::set<std::string>& terminals,
	std::set<std::string>& nonTerminals) {
		std::vector<Production> productions;
		std::ifstream infile(filename);
		std::string line;
		
		if (!infile) {
			std::cerr << "无法打开文件: " << filename << std::endl;
			return productions;
		}
		
		while (std::getline(infile, line)) {
			if (line.empty() || line[0] == '#') continue;
			
			std::istringstream iss(line);
			std::string left, arrow, token;
			iss >> left >> arrow; // A ->
			
			std::vector<std::string> rightSideTokens;
			while (iss >> token) {
				rightSideTokens.push_back(token);
			}
			
			// 拆分多个产生式（按 | 分割）
			std::vector<std::vector<std::string>> alternatives;
			std::vector<std::string> currentAlt;
			for (const auto& sym : rightSideTokens) {
				if (sym == "|") {
					if (!currentAlt.empty()) {
						alternatives.push_back(currentAlt);
						currentAlt.clear();
					}
				} else {
					currentAlt.push_back(sym);
				}
			}
			if (!currentAlt.empty()) {
				alternatives.push_back(currentAlt);
			}
			
			// 创建多个产生式
			for (const auto& alt : alternatives) {
				Production prod;
				prod.left = left;
				prod.right = alt;
				productions.push_back(prod);
				
				nonTerminals.insert(left);
				for (const auto& s : alt) {
					if (nonTerminals.find(s) == nonTerminals.end()) {
						terminals.insert(s);
					}
				}
			}
		}
		
		return productions;
	}

