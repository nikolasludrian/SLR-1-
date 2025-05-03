#pragma once
#include <string>
#include <vector>

enum class TokenType {
	KEYWORD,
	ID,
	NUM,
	SYM,
	END_OF_FILE
};

struct Token {
	TokenType type;
	std::string lexeme;
};

std::vector<Token> tokenize(const std::string& input);
void printTokens(const std::vector<Token>& tokens);

