#pragma once
#include<vector>
#include<utility>
#include<string>

using namespace std;

extern string symbolList[50];
extern vector<pair<string, int>> allSymbol;
void init();
bool getsym();