#include "lexer.h"
#include <iostream>
#include <unordered_set>
#include <cctype>

using namespace std;

// 关键字集合
unordered_set<string> keywords = {
	"if", "then", "else", "while", "do"
};

// 符号集合
unordered_set<string> symbols = {
	":=", "+", "*", "(", ")"
};

vector<Token> tokenize(const string& input) {
	vector<Token> tokens;
	size_t i = 0;
	
	while (i < input.length()) {
		if (isspace(input[i])) {
			++i;
			continue;
		}
		
		// 识别标识符或关键字
		if (isalpha(input[i]) || input[i] == '_') {
			string word;
			while (i < input.length() && (isalnum(input[i]) || input[i] == '_')) {
				word += input[i++];
			}
			if (keywords.count(word))
				tokens.push_back({TokenType::KEYWORD, word});
			else
				tokens.push_back({TokenType::ID, word});
		}
		// 识别数字
		else if (isdigit(input[i])) {
			string num;
			while (i < input.length() && isdigit(input[i])) {
				num += input[i++];
			}
			tokens.push_back({TokenType::NUM, num});
		}
		// 识别 := 双字符符号
		else if (input[i] == ':' && i + 1 < input.length() && input[i + 1] == '=') {
			tokens.push_back({TokenType::SYM, ":="});
			i += 2;
		}
		// 识别单字符符号
		else {
			string op(1, input[i]);
			if (symbols.count(op)) {
				tokens.push_back({TokenType::SYM, op});
				++i;
			} else {
				cerr << "Unknown character: " << input[i] << endl;
				exit(1);
			}
		}
	}
	
	// 添加文件结束符
	tokens.push_back({TokenType::END_OF_FILE, "$"});
	
	return tokens;
}

void printTokens(const vector<Token>& tokens) {
	for (const auto& token : tokens) {
		string typeStr;
		switch (token.type) {
			case TokenType::KEYWORD: typeStr = "KEYWORD"; break;
			case TokenType::ID: typeStr = "ID"; break;
			case TokenType::NUM: typeStr = "NUM"; break;
			case TokenType::SYM: typeStr = "SYM"; break;
			case TokenType::END_OF_FILE: typeStr = "EOF"; break;
		}
		cout << "<" << typeStr << ", " << token.lexeme << ">" << endl;
	}
}

