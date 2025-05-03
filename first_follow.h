#ifndef FIRST_FOLLOW_H
#define FIRST_FOLLOW_H

#include <string>
#include <vector>
#include <set>
#include <map>
using namespace std;
// 产生式结构
struct Production {
	string left;
	vector<string> right;
};

// 计算某个符号的FIRST 集
set<string> computeFirst(const vector<string>& symbols,
	const map<string, set<string>>& firstSets);

// 计算 First 集
map<string, set<string>> computeFirstSets(const vector<Production>& productions) ;

// 计算Follow 集
map<string, set<string>> computeFollowSets(
	const vector<Production>& productions,
	const map<string, set<string>>& firstSet
	) ;

// 打印 FIRST 或 FOLLOW 集
void printSet(const map<string, set<string>>& sets, const string& name);

#endif // FIRST_FOLLOW_H

