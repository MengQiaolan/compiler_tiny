#include<iostream>
#include "word_analysis.h"
#include"grammar_analysis.h"
using namespace std;

#define random(x) (rand()%x)

int main() {
	init();
	while (getsym());
	for (int i = 0; i < allSymbol.size(); ++i) {
		cout << allSymbol[i].first << "	" << symbolList[allSymbol[i].second] << endl;
	}
	_PROGRAM_();
	getchar();
	getchar();
	return 0;
}