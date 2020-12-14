#include<iostream>
#include<string>
#include<algorithm>
#include<vector>
#include<utility>
#include"word_analysis.h"
#include"symbol.h"
#include<fstream>

#define EXPR 1

#define PLUS 16
#define MINUS 16
#define TIMES 17
#define DIV 17
#define AND 18
#define OR 18
#define NOT 19

#define LPA 2
#define RPA 3
#define LBR 4
#define RBR 5

using namespace std;

// 当前处理的单词位置
int index = 0;
// 当前作用域
vector<string> SCOPE = { "global" };
string atObject;
ofstream midCodeFile;

int lableNo = 0;
int tempVarNo = 0;

class SYMBOL_TABLE_ITEM;
class SYMBOL_TABLE;
class EXPR_STACK;
void _CONST_();
void _CONSTDEF_();
void _VAR_();
void _VARDEF_();
void _OBJECT_();
void _FUNCTION_();
void _STATEMENT_();
void _STATEMENTS_();
void _IF_();
void _REPEAT_();
void _INPUT_();
void _OUTPUT_();
void _RETURN_();
void _ASSIGN_();
void _CALL_();
void _CONDITION_();
void _EXPRESS_();
void _TERM_();
void _TINY_TERM_();
void _DOUBLE_TINY_TERM_();
void _FACTOR_();
void _PRINT_MIDCODE_(const int& param1,
	const int& param2,
	const int& param3,
	const string& param4,
	const string& param5,
	const string& param6);

class SYMBOL_TABLE_ITEM {
public:
	int type; //var object function 
	string name;
	bool isConst; //const
	bool isArray; //array
	int size = 0; //size of array
	vector<pair<string, int>> sonVar = {}; // object.var
	vector<string> scope; //作用域
	// var, object
	SYMBOL_TABLE_ITEM(const int& t, const string& n, const bool& con) :
		type(t), name(n), isConst(false), isArray(false), size(0), scope(SCOPE) {}
	// array
	SYMBOL_TABLE_ITEM(const int& t, const string& n, const int& l) :
		type(t), name(n), isConst(false), isArray(true), size(l), scope(SCOPE) {}
	// function
	SYMBOL_TABLE_ITEM(const int& t, const string& n) :
		type(t), name(n), isConst(false), isArray(false), size(0), scope(SCOPE) {}
	// object
	SYMBOL_TABLE_ITEM(const string& n) :
		type(OBJECTSY), name(n), isConst(false), isArray(false), size(0), scope(SCOPE) {}
};

class SYMBOL_TABLE {
public:
	vector<SYMBOL_TABLE_ITEM> TABLE;
	//var
	void addItem(const int& t, const string& n, const bool& con) {
		// object
		if (atObject != "") {
			getItemByName(atObject).sonVar.push_back({ n, VARSY });
		}
		// Var
		else {
			if (!findByName(n)) TABLE.push_back(SYMBOL_TABLE_ITEM(t, n, con));
			else cout << "重复命名" << endl;
		}
	}
	//array
	void addItem(const int& t, const string& n, const int& l) {
		if (!findByName(n)) TABLE.push_back(SYMBOL_TABLE_ITEM(t, n, l));
		else cout << "重复命名" << endl;
	}
	bool findByName(const string& n) {
		for (auto i = TABLE.begin(); i != TABLE.end(); ++i) {
			if (i->name == n) return true;
		}
		return false;
	}
	int getTypeByName(const string& n) {
		for (auto i = TABLE.begin(); i != TABLE.end(); ++i) {
			if (i->name == n) return i->type;
		}
		return -1;
	}
	SYMBOL_TABLE_ITEM& getItemByName(const string& n) {
		for (auto i = TABLE.begin(); i != TABLE.end(); ++i) {
			if (i->name == n) return *i;
		}
		{ cout << "error：" << endl; exit(-1); }
	}
};

class EXPR_STACK {
public:
	vector<pair<string, int>> STA_OPERATOR;
	vector<string> STA_OPERAND;
	void pushOperator(const string& s, const int& pri) {
		if (pri == LPA || pri == LBR) {
			STA_OPERATOR.push_back({ s, pri });
			return;
		}
		string s0 = STA_OPERATOR.back().first;
		int pri0 = STA_OPERATOR.back().second;

		if (pri == RPA) {
			while (LPA != pri0) {
				oneCalculate();
				s0 = STA_OPERATOR.back().first;
				pri0 = STA_OPERATOR.back().second;
			}
			STA_OPERATOR.pop_back();
			return;
		}
		if (pri == RBR) {
			while (LBR != pri0) {
				oneCalculate();
				s0 = STA_OPERATOR.back().first;
				pri0 = STA_OPERATOR.back().second;
			}
			oneCalculate();
			return;
		}
		while (pri <= pri0) {
			oneCalculate();
			s0 = STA_OPERATOR.back().first;
			pri0 = STA_OPERATOR.back().second;
		}
		STA_OPERATOR.push_back({ s, pri });
	}
	void pushOperand(const string& s) {
		STA_OPERAND.push_back(s);
	}
	string finish() {
		while (STA_OPERATOR.back().second != EXPR) {
			oneCalculate();
		}
		string res = STA_OPERAND.back();
		STA_OPERAND.pop_back();
		// STA_OPERAND.pop_back();
		STA_OPERATOR.pop_back();
		return res;
	}
	void oneCalculate() {
		_PRINT_MIDCODE_(ASSIGNSY, 1, 0, *(STA_OPERAND.end() - 2), *(STA_OPERAND.end() - 1), (*(STA_OPERATOR.end() - 1)).first);
		STA_OPERATOR.pop_back();
		STA_OPERAND.pop_back();
		STA_OPERAND.pop_back();
		STA_OPERAND.push_back("@t" + to_string(tempVarNo));
	}
	void init() {
		STA_OPERATOR.push_back({ "expr", EXPR });
	}
};

SYMBOL_TABLE symbolTable;
EXPR_STACK sta;

int getType(int i) {
	try {
		return allSymbol.at(i).second;
	}
	catch (...) {
		exit(0);
	}
}
int getValue(int i) {
	try {
		if (allSymbol.at(i).second == CHAR2SY) return allSymbol.at(i).first[0];
		else if (allSymbol.at(i).second == UNSIGNSY) return atoi(allSymbol.at(i).first.c_str());
		else { cout << "error：" << endl; exit(-1); }
	}
	catch (...) {
		exit(0);
	}
}
string getString(int i) {
	try {
		return allSymbol.at(i).first;
	}
	catch (...) {
		exit(0);
	}
}

void _PROGRAM_() {
	midCodeFile.open("midcode.txt");
	if (!midCodeFile.is_open()) {
		cout << "无法创建输出文件！" << endl;
		exit(0);
	}
	if (getType(index) == OBJECTSY) {
		_OBJECT_();
	}
	if (getType(index) == CONSTSY) {
		_CONST_();
	}
	if (getType(index) == VARSY) {
		_VAR_();
	}
	if (getType(index) == FUNCTIONSY && getType(index + 1) != MAINSY) {
		_FUNCTION_();
	}
	if (getType(index) == FUNCTIONSY && getType(index + 1) == MAINSY) {
		_FUNCTION_();
	}
	midCodeFile.close();
	/*while (index < allSymbol.size()) {
		if (getType(index) == CONSTSY) {

		}
		else if (getType(index) == VARSY) {
			_VAR_();
		}
		else if (getType(index) == OBJECTSY) {
			_OBJECT_();
		}
		else if (getType(index) == IFSY) {
			_IF_();
		}
		else if (getType(index) == FUNCTIONSY) {
			_FUNCTION_();
		}
		else if (getType(index) == INPUTSY) {
			_INPUT_();
		}
		else if (getType(index) == OUTPUTSY) {
			_OUTPUT_();
		}
		else _STATEMENT_();
	}*/
}

void _CONST_() {
	_CONSTDEF_();
	if (getType(index++) != SEMISY) { cout << "error：" << endl; exit(-1); }
	while (getType(index) == CONSTSY) {
		_CONSTDEF_();
		if (getType(index++) != SEMISY) { cout << "error：" << endl; exit(-1); }
	}
}

void _CONSTDEF_() {
	if (getType(index++) != CONSTSY) { cout << "error：" << endl; exit(-1); }
	while (true) {
		if (getType(index) != IDENSY || getType(index + 1) != ASSIGNSY) { cout << "error：" << endl; exit(-1); }
		// 创建符号表项
		index += 2;
		if (false);
		else if (getType(index) == CHAR2SY) {
			_PRINT_MIDCODE_(CONSTSY, getValue(index), 0, getString(index - 2), "", "");
			symbolTable.addItem(CHAR2SY, getString(index - 2), true);
		}
		else if (getType(index) == UNSIGNSY) {
			_PRINT_MIDCODE_(CONSTSY, getValue(index), 0, getString(index - 2), "", "");
			symbolTable.addItem(SIGNSY, getString(index - 2), true);
		}
		else if (getType(index) == MINUSSY || getType(index + 1) == UNSIGNSY) {
			++index;
			_PRINT_MIDCODE_(CONSTSY, -getValue(index), 0, getString(index - 3), "", "");
			symbolTable.addItem(SIGNSY, getString(index - 3), true);
		}
		else if (getType(index) == PLUSSY || getType(index + 1) == UNSIGNSY) {
			++index;
			_PRINT_MIDCODE_(CONSTSY, getValue(index), 0, getString(index - 3), "", "");
			symbolTable.addItem(SIGNSY, getString(index - 3), true);
		}
		else { cout << "error：" << endl; exit(-1); }
		++index;

		if (getType(index) == SEMISY) {
			break;
		}
		if (getType(index) == COMMASY) {
			++index;
			continue;
		}
		else { cout << "error：" << endl; exit(-1); }
	}
}

void _VAR_() {
	_VARDEF_();
	if (getType(index++) != SEMISY) { cout << "error：" << endl; exit(-1); }
	while (getType(index) == VARSY) {
		_VARDEF_();
		if (getType(index++) != SEMISY) { cout << "error：" << endl; exit(-1); }
	}
}

void _VARDEF_() {
	if (getType(index++) != VARSY) { cout << "error：" << endl; exit(-1); }
	while (true) {
		if (getType(index) != IDENSY) { cout << "error：" << endl; exit(-1); }
		// ＜标识符＞‘[’＜无符号整数＞‘]’
		if (getType(index + 1) == LBRASY) {
			index += 2;
			if (getType(index) == UNSIGNSY && getType(index + 1) == RBRASY) {
				_PRINT_MIDCODE_(VARSY, getValue(index), 0, getString(index - 2), "", "");
				symbolTable.addItem(getType(index - 2), getString(index - 2), getValue(index));
			}
			else { cout << "error：" << endl; exit(-1); }
			index += 2;
			if (getType(index) == SEMISY) {
				break;
			}
			if (getType(index) == COMMASY) {
				++index;
				continue;
			}
		}
		// ;
		else if (getType(index + 1) == SEMISY) {
			_PRINT_MIDCODE_(VARSY, 0, 0, getString(index), "", "");
			symbolTable.addItem(VARSY, getString(index), false);
			++index;
			break;
		}
		// ,
		else if (getType(index + 1) == COMMASY) {
			_PRINT_MIDCODE_(VARSY, 0, 0, getString(index), "", "");
			symbolTable.addItem(VARSY, getString(index), false);
			index += 2;
			continue;
		}
		else { cout << "error：" << endl; exit(-1); }
	}
}

void _OBJECT_() {
	if (getType(index++) != OBJECTSY) { cout << "error：" << endl; exit(-1); }
	if (getType(index) != IDENSY || getType(index + 1) != ASSIGNSY || getType(index + 2) != LCURSY) { cout << "error：" << endl; exit(-1); }
	_PRINT_MIDCODE_(OBJECTSY, 0, 0, getString(index), "", "");
	symbolTable.addItem(OBJECTSY, getString(index), false);
	atObject = getString(index);
	index += 3;
	_VAR_();
	atObject = "";
	if (getType(index++) != RCURSY) { cout << "error：" << endl; exit(-1); }
}

void _FUNCTION_() {
	if (getType(index++) != FUNCTIONSY) { cout << "error：" << endl; exit(-1); }
	_PRINT_MIDCODE_(FUNCTIONSY, 0, 0, getString(index), "", "");
	if (true) {
		if ((getType(index) != IDENSY && getType(index) != MAINSY) || getType(index + 1) != LPASY) { cout << "error：" << endl; exit(-1); }
		symbolTable.addItem(FUNCTIONSY, getString(index), false);
		SCOPE.push_back(getString(index));
		index += 2;
		// param list
		while (getType(index) != RPASY) {
			if (getType(index) != VARSY) { cout << "error：" << endl; exit(-1); }
			if (getType(++index) != IDENSY) { cout << "error：" << endl; exit(-1); }
			_PRINT_MIDCODE_(VARSY, 1, 0, getString(index), "", "");
			symbolTable.addItem(VARSY, getString(index), false);
			if (getType(++index) == COMMASY) ++index;
		}
		if (getType(++index) != LCURSY) { cout << "error：" << endl; exit(-1); }
		++index;
		while (getType(index) != RCURSY) {
			if (getType(index) == OBJECTSY) _OBJECT_();
			if (getType(index) == CONSTSY) _CONST_();
			if (getType(index) == VARSY) _VAR_();
			_STATEMENT_();
		}
		++index;
		SCOPE.pop_back();
		_PRINT_MIDCODE_(FUNCTIONSY, 1, 0, "", "", "");
	}
}

void _STATEMENT_() {
	// ＜条件语句＞
	if (getType(index) == IFSY) {
		_IF_();
	}
	// ＜循环语句＞
	else if (getType(index) == REPEATSY) {
		_REPEAT_();
	}
	//   ‘{’＜语句列＞‘}’
	else if (getType(index) == LCURSY) {
		_STATEMENTS_();
	}
	// ＜读语句＞;
	else if (getType(index) == INPUTSY) {
		_INPUT_();
		if (getType(index++) != SEMISY) { cout << "error：" << endl; exit(-1); }
	}
	// ＜写语句＞;
	else if (getType(index) == OUTPUTSY) {
		_OUTPUT_();
		if (getType(index++) != SEMISY) { cout << "error：" << endl; exit(-1); }
	}
	// ＜空＞;
	else if (getType(index) == SEMISY) {
		if (getType(index++) != SEMISY) { cout << "error：" << endl; exit(-1); }
	}
	// ＜返回语句＞;
	else if (getType(index) == RETURNSY) {
		_RETURN_();
		if (getType(index++) != SEMISY) { cout << "error：" << endl; exit(-1); }
	}
	// ＜函数调用语句＞;
	else if (getType(index) == LPASY) {
		_CALL_();
		if (getType(index++) != SEMISY) { cout << "error：" << endl; exit(-1); }
	}
	// ＜赋值语句＞;
	else {
		_ASSIGN_();
		if (getType(index++) != SEMISY) { cout << "error：" << endl; exit(-1); }
	}

}

void _STATEMENTS_() {
	if (getType(index++) != LCURSY) { cout << "error：" << endl; exit(-1); }
	while (getType(index) != RCURSY) {
		_STATEMENT_();
	}
	++index;
}

void _IF_() {
	if (getType(index++) != IFSY) { cout << "error：" << endl; exit(-1); }
	if (getType(index++) != LPASY) { cout << "error：" << endl; exit(-1); }
	int tempLabel = ++lableNo;
	_CONDITION_();
	_PRINT_MIDCODE_(IFSY, 3, tempLabel, "", "", "");

	if (getType(index++) != RPASY) { cout << "error：" << endl; exit(-1); }
	_STATEMENT_();

	if (getType(index) == ELSESY) {
		int tempLabel2 = ++lableNo;
		_PRINT_MIDCODE_(IFSY, 5, tempLabel2, "", "", "");
		_PRINT_MIDCODE_(IFSY, 4, tempLabel, "", "", "");
		++index;
		_STATEMENT_();
		_PRINT_MIDCODE_(IFSY, 4, tempLabel2, "", "", "");
	}
	else {
		_PRINT_MIDCODE_(IFSY, 4, tempLabel, "", "", "");
	}
}

void _REPEAT_() {
	if (getType(index++) != REPEATSY) { cout << "error：" << endl; exit(-1); }
	if (getType(index++) != LPASY) { cout << "error：" << endl; exit(-1); }
	int tempLabel = ++lableNo;
	_PRINT_MIDCODE_(IFSY, 4, tempLabel, "", "", "");
	int tempLabel2 = ++lableNo;
	_CONDITION_();
	_PRINT_MIDCODE_(IFSY, 3, tempLabel2, "", "", "");
	if (getType(index++) != RPASY) { cout << "error：" << endl; exit(-1); }
	_STATEMENT_();
	_PRINT_MIDCODE_(IFSY, 5, tempLabel, "", "", "");
	_PRINT_MIDCODE_(IFSY, 4, tempLabel2, "", "", "");
}

void _INPUT_() {
	if (getType(index++) != INPUTSY) { cout << "error：" << endl; exit(-1); }
	if (getType(index++) != LPASY) { cout << "error：" << endl; exit(-1); }
	while (getType(index) != RPASY) {
		if (getType(index++) != IDENSY) { cout << "error：" << endl; exit(-1); }
		_PRINT_MIDCODE_(INPUTSY, 0, 0, getString(index - 1), "", "");
		if (getType(index) != COMMASY && getType(index) != RPASY) { cout << "error：" << endl; exit(-1); }
		if (getType(index) == COMMASY && getType(index + 1) == RPASY) { cout << "error：" << endl; exit(-1); }
		if (getType(index) == COMMASY) ++index;
	}
	index += 1;
}

void _OUTPUT_() {
	if (getType(index++) != OUTPUTSY) { cout << "error：" << endl; exit(-1); }
	if (getType(index++) != LPASY) { cout << "error：" << endl; exit(-1); }
	while (getType(index) != RPASY) {
		if (getType(index++) != IDENSY) { cout << "error：" << endl; exit(-1); }
		_PRINT_MIDCODE_(OUTPUTSY, 0, 0, getString(index - 1), "", "");
		if (getType(index) != COMMASY && getType(index) != RPASY) { cout << "error：" << endl; exit(-1); }
		if (getType(index) == COMMASY && getType(index + 1) == RPASY) { cout << "error：" << endl; exit(-1); }
		if (getType(index) == COMMASY) ++index;
	}
	index += 1;
}

void _RETURN_() {
	if (getType(index++) != RETURNSY) { cout << "error：" << endl; exit(-1); }
	if (getType(index) == RPASY) {
		++index;
		if (getType(index) != LPASY) {
			_PRINT_MIDCODE_(FUNCTIONSY, 2, 0, "0", "", "");
		}
		else {
			sta.init();
			_EXPRESS_();
			string res = sta.finish();
			_PRINT_MIDCODE_(FUNCTIONSY, 2, 0, res, "", "");
			if (getType(index++) != LPASY) { cout << "error：" << endl; exit(-1); }
		}
	}
}

// ＜赋值语句＞:: = 
//		＜标识符＞＝＜表达式＞ | 
//		＜标识符＞‘[’＜表达式＞‘]’ = ＜表达式＞ |
//		＜标识符＞.＜标识符＞＝＜表达式＞
void _ASSIGN_() {
	if (getType(index++) != IDENSY) { cout << "error：" << endl; exit(-1); }
	string tempIden = getString(index - 1);
	if (getType(index) == ASSIGNSY) {
		++index;
		sta.init();
		_EXPRESS_();
		string res = sta.finish();
		_PRINT_MIDCODE_(ASSIGNSY, 2, 0, tempIden, res, "");
	}
	else if (getType(index) == LBRASY) {
		++index;
		sta.init();
		_EXPRESS_();
		string res = sta.finish();
		if (getType(index++) != RBRASY) { cout << "error：" << endl; exit(-1); }
		if (getType(index++) != ASSIGNSY) { cout << "error：" << endl; exit(-1); }
		sta.init();
		_EXPRESS_();
		string res2 = sta.finish();
		_PRINT_MIDCODE_(ASSIGNSY, 3, 0, tempIden, res, res2);
	}
	else if (getType(index) == DOTSY) {
		++index;
		if (getType(index++) != IDENSY) { cout << "error：" << endl; exit(-1); }
		string tempIden2 = getString(index - 1);
		if (getType(index++) != ASSIGNSY) { cout << "error：" << endl; exit(-1); }
		sta.init();
		_EXPRESS_();
		string res = sta.finish();
		_PRINT_MIDCODE_(ASSIGNSY, 4, 0, tempIden, tempIden2, res);
	}
	else { cout << "error：" << endl; exit(-1); }

	// if (getType(index++) != ASSIGNSY) { cout << "error：" << endl; exit(-1); }
}

void _CALL_() {
	if (getType(index) != IDENSY) { cout << "error：" << endl; exit(-1); }
	if (symbolTable.getTypeByName(getString(index++)) != FUNCTIONSY) { cout << "error：" << endl; exit(-1); }
	string tempIden = getString(index - 1);
	if (getType(index) == LPASY) {
		++index;
		while (getType(index) != RPASY) {
			sta.init();
			_EXPRESS_();
			string res = sta.finish();
			_PRINT_MIDCODE_(FUNCTIONSY, 3, 0, res, "", "");
			if (getType(index) != COMMASY && getType(index) != RPASY) { cout << "error：" << endl; exit(-1); }
			if (getType(index) == COMMASY && getType(index + 1) == RPASY) { cout << "error：" << endl; exit(-1); }
			if (getType(index) == COMMASY) ++index;
		}
	}
	_PRINT_MIDCODE_(FUNCTIONSY, 4, 0, tempIden, "", "");
	string tempNo = "@" + to_string(++tempVarNo);
	sta.pushOperand(tempNo);
	_PRINT_MIDCODE_(FUNCTIONSY, 5, 0, tempNo, "", "");
}

void _CONDITION_() {
	sta.init();
	_EXPRESS_();
	string res = sta.finish();
	if ((getType(index) < LESSY || getType(index) > EQLSY) && getType(index) != RPASY) { cout << "error：" << endl; exit(-1); }
	if (getType(index) == RPASY) {
		_PRINT_MIDCODE_(IFSY, 1, 0, res, "", "");
		++index;
	}
	else {
		string op = getString(index++);
		sta.init();
		_EXPRESS_();
		string res2 = sta.finish();
		_PRINT_MIDCODE_(IFSY, 2, 0, res, op, res2);
	}
}

void _EXPRESS_() {
	//sta.pushOperand("expr");
	//sta.pushOperator("expr", EXPR);
	if (getType(index) == PLUSSY || getType(index) == MINUSSY) {
		if (getType(index) == MINUSSY) {
			sta.pushOperand("0");
			sta.pushOperator("-", MINUS);
		}
		++index;
	}
	_TERM_();
	while (getType(index) == PLUSSY || getType(index) == MINUSSY) {
		if (getType(index) == MINUSSY) sta.pushOperator("-", MINUS);
		else sta.pushOperator("+", PLUS);
		++index;
		_TERM_();
	}
}

// ＜项＞ ::= ＜次级项＞{＜乘除运算符＞＜次级项＞}
void _TERM_() {
	_TINY_TERM_();
	while (getType(index) == TIMESSY || getType(index) == DIVSY) {
		if (getType(index) == TIMESSY) sta.pushOperator("*", TIMES);
		else sta.pushOperator("/", DIV);
		++index;
		_TINY_TERM_();
	}
}

// <次级项> :: = <次次级项>{ <与或><次次级项> }
void _TINY_TERM_() {
	_DOUBLE_TINY_TERM_();
	while (getType(index) == ANDSY || getType(index) == ORSY) {
		if (getType(index) == ANDSY) sta.pushOperator("&", AND);
		else sta.pushOperator("|", OR);
		++index;
		_DOUBLE_TINY_TERM_();
	}
}

// <次次级项> :: = [ <非> ]<因子>
void _DOUBLE_TINY_TERM_() {
	if (getType(index) == NOTSY) {
		++index;
		sta.pushOperand("null");
		sta.pushOperator("~", NOT);
	}
	_FACTOR_();
}

void _FACTOR_() {
	// ＜标识符＞‘[’＜表达式＞‘]’
	if (getType(index + 1) == LBRASY) {
		sta.pushOperand(getString(index));
		sta.pushOperator("[", LBR);
		index += 2;
		_EXPRESS_();
		sta.pushOperator("]", RBR);
		if (getType(index++) != RBRASY) { cout << "error：" << endl; exit(-1); }
	}
	//‘(’＜表达式＞‘)’
	else if (getType(index) == LPASY) {
		++index;
		sta.pushOperator("(", LPA);
		_EXPRESS_();
		sta.pushOperator(")", RPA);
		if (getType(index++) != RPASY) { cout << "error：" << endl; exit(-1); }
	}
	//  ＜标识符＞.＜标识符＞
	else if (getType(index + 1) == DOTSY) {
		_PRINT_MIDCODE_(ASSIGNSY, 1, 0, getString(index), getString(index + 2), ".");
		sta.pushOperand("@t" + to_string(tempVarNo));
		index += 3;
	}
	// ＜函数调用语句＞
	else if (symbolTable.getTypeByName(getString(index)) == FUNCTIONSY) {
		_CALL_();
		++index;
	}
	// ＜标识符＞
	else if (getType(index) == IDENSY) {
		sta.pushOperand(getString(index));
		++index;
	}
	// <字符>
	else if (getType(index) == CHAR2SY) {
		sta.pushOperand(to_string(int(getString(index)[0])));
		++index;
	}
	// <整数>
	else if (getType(index) == UNSIGNSY) {
		sta.pushOperand(getString(index));
		++index;
	}
	else { cout << "error：" << endl; { cout << "error：" << endl; { cout << "error：" << endl; exit(-1); } } }
}

void _PRINT_MIDCODE_(
	const int& param1,
	const int& param2,
	const int& param3,
	const string& param4,
	const string& param5,
	const string& param6
) {
	if (param1 == CONSTSY) {
		cout << "const " << param4 << " " << param2 << endl;
		midCodeFile << "const " << param4 << " " << param2 << endl;
	}
	else if (param1 == VARSY) {
		if (atObject != "") {
			cout << "object " << atObject << " " << param4 << endl;
			midCodeFile << "object " << atObject << " " << param4 << endl;
		}
		else if (param2 == 0) {
			cout << "var " << param4 << endl;
			midCodeFile << "var " << param4 << endl;
		}
		// 参数
		else if (param2 == 1) {
			cout << "para var " << param4 << endl;
			midCodeFile << "para var " << param4 << endl;
		}
		else {
			cout << "array " << param4 << " " << param2 << endl;
			midCodeFile << "array " << param4 << " " << param2 << endl;
		}
	}
	else if (param1 == OBJECTSY) {
		cout << "object " << param4 << endl;
		midCodeFile << "object " << param4 << endl;
	}
	else if (param1 == FUNCTIONSY) {
		if (param2 == 0) {
			cout << "function " << param4 << endl;
			midCodeFile << "function " << param4 << endl;
		}
		else if (param2 == 1) {
			cout << "RET 0" << endl;
			midCodeFile << "RET 0" << endl;
		}
		else if (param2 == 2) {
			cout << "RET " << param4 << endl;
			midCodeFile << "RET " << param4 << endl;
		}
		else if (param2 == 3) {
			cout << "para " << param4 << endl;
			midCodeFile << "para " << param4 << endl;
		}
		else if (param2 == 4) {
			cout << "call " << param4 << endl;
			midCodeFile << "call " << param4 << endl;
		}
		else if (param2 == 5) {
			cout << param4 << " = RET" << endl;
			midCodeFile << param4 << " = RET" << endl;
		}
	}
	else if (param1 == INPUTSY) {
		cout << "input " << param4 << endl;
		midCodeFile << "input " << param4 << endl;
	}
	else if (param1 == OUTPUTSY) {
		cout << "output " << param4 << endl;
		midCodeFile << "output " << param4 << endl;
	}
	else if (param1 == ASSIGNSY) {
		if (param2 == 1) {
			if (param6 == "[") {
				cout << "@t" << ++tempVarNo << " = " << param4 << " [ " << param5 << " ]" << endl;
				midCodeFile << "@t" << ++tempVarNo << " = " << param4 << " [ " << param5 << " ]" << endl;
			}
			else if (param4 == "null") {
				cout << "@t" << ++tempVarNo << " = " << param6 << " " << param5 << endl;
				midCodeFile << "@t" << ++tempVarNo << " = " << param6 << " " << param5 << endl;
			}
			else {
				cout << "@t" << ++tempVarNo << " = " << param4 << " " << param6 << " " << param5 << endl;
				midCodeFile << "@t" << ++tempVarNo << " = " << param4 << " " << param6 << " " << param5 << endl;
			}
		}
		if (param2 == 2) {
			cout << param4 << " = " << param5 << endl;
			midCodeFile << param4 << " = " << param5 << endl;
		}
		if (param2 == 3) {
			cout << param4 << "[ " << param5 << " ] = " << param6 << endl;
			midCodeFile << param4 << "[ " << param5 << " ] = " << param6 << endl;
		}
		if (param2 == 4) {
			cout << param4 << " . " << param5 << " = " << param6 << endl;
			midCodeFile << param4 << " . " << param5 << " = " << param6 << endl;
		}
	}
	else if (param1 == IFSY) {
		if (param2 == 0) {
			cout << "BZ label_" << param3 << endl;
			midCodeFile << "BZ label_" << param3 << endl;
		}
		else if (param2 == 1) {
			cout << param4 << " == 1" << endl;
			midCodeFile << param4 << " == 1" << endl;
		}
		else if (param2 == 2) {
			cout << param4 << " " << param5 << " " << param6 << endl;
			midCodeFile << param4 << " " << param5 << " " << param6 << endl;
		}
		else if (param2 == 3) {
			cout << "BZ label_" << param3 << endl;
			midCodeFile << "BZ label_" << param3 << endl;
		}
		else if (param2 == 4) {
			cout << "label_" << param3 << ":" << endl;
			midCodeFile << "label_" << param3 << ":" << endl;
		}
		else if (param2 == 5) {
			cout << "GOTO label_" << param3 << endl;
			midCodeFile << "GOTO label_" << param3 << endl;
		}
		else;
	}
}