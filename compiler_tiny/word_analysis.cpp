#include<fstream>
#include<sstream>
#include<iostream>
#include<string>
#include<vector>
#include"symbol.h"
using namespace std;


string symbolList[50]; //保留字表
vector <pair<string, int>> allSymbol;

string lineBuffer; //行缓冲区
int linePtr; //行指针
int lineSize; //行长度
int indexOfLine; //行编号

string tokenBuffer; //token缓冲区
int tokenPtr; //token缓冲区指针
int symbol; //token类型

char ch; //当前读到的字符
int num; //当前读到的整数

ifstream inFile; //源代码文件
ofstream outfile; //词法分析结果文件
string filename; //源文件

//初始化保留字和特殊字符,读入文件
void init() {
	if (true) {
		symbolList[CONSTSY] = "CONSTSY";
		symbolList[OBJECTSY] = "OBJECTSY";
		symbolList[VARSY] = "VARSY";
		symbolList[FUNCTIONSY] = "FUNCTIONSY";
		symbolList[MAINSY] = "MAINSY";
		symbolList[IFSY] = "IFSY";
		symbolList[ELSESY] = "ELSESY";
		symbolList[REPEATSY] = "REPEATSY";
		// symbolList[FORSY] = "FORSY";
		symbolList[INPUTSY] = "INPUTSY";
		symbolList[OUTPUTSY] = "OUTPUTSY";
		symbolList[RETURNSY] = "RETURNSY";
		symbolList[PLUSSY] = "PLUSSY";
		symbolList[MINUSSY] = "MINUSSY";
		symbolList[TIMESSY] = "TIMESSY";
		symbolList[DIVSY] = "DIVSY";
		symbolList[LESSY] = "LESSY";
		symbolList[LEESY] = "LEEY";
		symbolList[GRTSY] = "GRTSY";
		symbolList[GRESY] = "GRESY";
		symbolList[NEQSY] = "NEQSY";
		symbolList[EQLSY] = "EQLSY";
		symbolList[SGQSY] = "SGQSY";
		symbolList[QUOSY] = "QUOSY";
		symbolList[LPASY] = "LPASY";
		symbolList[RPASY] = "RPASY";
		symbolList[LBRASY] = "LBRASY";
		symbolList[RBRASY] = "RBRASY";
		symbolList[LCURSY] = "LCURSY";
		symbolList[RCURSY] = "RCURSY";
		symbolList[COMMASY] = "COMMASY";
		symbolList[SEMISY] = "SEMISY";
		symbolList[ASSIGNSY] = "ASSIGNSY";
		symbolList[IDENSY] = "IDENSY";
		symbolList[UNSIGNSY] = "UNSIGNSY";
		symbolList[SIGNSY] = "SIGNSY";
		symbolList[STRINGSY] = "STRINGSY";
		symbolList[CHAR2SY] = "CHAR2SY";
		symbolList[ERROR2] = "ERROR2";
		symbolList[DOTSY] = "DOTSY";
		symbolList[ANDSY] = "ANDSY";
		symbolList[ORSY] = "ORSY";
		symbolList[NOTSY] = "NOTSY";
	}

	indexOfLine = 0;
	cout << "请输入源代码文件名：" << endl;
	//cin >> filename;
	filename = "code.txt";
	inFile.open(filename);
	if (!inFile.is_open()) {
		cout << "找不到输入文件！" << endl;
		exit(0);
	}
	outfile.open("resOfWordAnalysis.txt");
	if (!outfile.is_open()) {
		cout << "无法创建输出文件！" << endl;
		exit(0);
	}
}

//读一个字符
char getch() {
	if (linePtr == lineSize) {
		indexOfLine++;
		if (getline(inFile, lineBuffer)) {
			lineSize = lineBuffer.size();
			linePtr = 0;
			return('\n');
		}
		else
			return EOF;
	}
	else linePtr++;
	//cout << "读到  " << line[linePtr - 1] << endl;
	return lineBuffer[linePtr - 1];
}

//清空token，准备读取下一个单词
void next() {
	tokenBuffer = "";
	tokenPtr = 0;
	ch = getch();
}

//判断是否已经读完文件
bool isEND() {
	if (ch == EOF) {
		cout << "END OF READING" << endl;
		inFile.close();
		outfile.close();
		allSymbol.push_back({ "",-1 });
		return true;
	}
	return false;
}

//判断token是否是关键字
int isKEY() {
	if (tokenBuffer == "const") return CONSTSY;
	if (tokenBuffer == "object") return OBJECTSY;
	if (tokenBuffer == "var") return VARSY;
	if (tokenBuffer == "function") return FUNCTIONSY;
	if (tokenBuffer == "main") return MAINSY;
	if (tokenBuffer == "if") return IFSY;
	if (tokenBuffer == "else") return ELSESY;
	if (tokenBuffer == "repeat") return REPEATSY;
	// if (tokenBuffer == "for") return FORSY;
	if (tokenBuffer == "input") return INPUTSY;
	if (tokenBuffer == "output") return OUTPUTSY;
	if (tokenBuffer == "return") return REPEATSY;
	return 34;
}

//打印错误
void printerror(int op) {
	if (op == 1) cout << "#Line  " << indexOfLine << " ： 存在不合法的字符 [!]" << endl;
	if (op == 2) cout << "#Line  " << indexOfLine << " ： 缺少右单引号 " << endl;
	if (op == 3) cout << "#Line  " << indexOfLine << " ： 字符串中存在不合法字符 '" << ch << "'" << endl;
	if (op == 4) cout << "#Line  " << indexOfLine << " ： 字符串缺少[\"] " << endl;

}

//输出词法分析结果到控制台和文件中
void printRes() {
	// cout << indexOfLine << "	" << symbolList[symbol] << "		" << tokenBuffer << endl;
	outfile << indexOfLine << "	" << symbolList[symbol] << "		" << tokenBuffer << endl;
	allSymbol.push_back({ tokenBuffer, symbol });
	if (indexOfLine > 9999999) {
		cout << "编译失败！" << endl;
		exit(0);
	}
}

//词法分析
bool getsym() {
	next();
	//忽略单词间的空格，制表符，换行符
	while (ch == ' ' || ch == '	' || ch == '\n') {
		ch = getch();
		continue;
	}
	if (!isEND()) {
		//-A-保留字,标识符
		if (isalpha(ch) || ch == '_') {
			do {
				tokenBuffer += tolower(ch);
				ch = getch();
			} while (isalnum(ch) || ch == '_');
			//如果ch是换行符，则不需要回退行指针（回退会出现linePtr =  -1）
			if (ch >= 32 && ch <= 126)linePtr--;
			symbol = isKEY();
			printRes();
		}
		//-A-无符号整数
		else if (isdigit(ch)) {
			num = 0;
			do {
				num = num * 10 + (ch - '0');
				tokenBuffer += tolower(ch);
				ch = getch();
			} while (isdigit(ch));
			//如果ch是换行符，则不需要回退行指针（回退会出现linePtr =  -1）
			if (ch >= 32 && ch <= 126)linePtr--;
			symbol = 35;
			printRes();
		}
		//-A-双字符分界符 < > ! =
		else if (ch == '<') {
			tokenBuffer += tolower(ch);
			ch = getch();
			if (ch == '=') {
				tokenBuffer += tolower(ch);
				symbol = 18;
			}
			else {
				//如果ch是换行符，则不需要回退行指针（回退会出现linePtr =  -1）
				if (ch >= 32 && ch <= 126)linePtr--;
				symbol = 17;
			}
			printRes();
		}
		else if (ch == '>') {
			tokenBuffer += tolower(ch);
			ch = getch();
			if (ch == '=') {
				tokenBuffer += tolower(ch);
				symbol = 20;
			}
			else {
				//如果ch是换行符，则不需要回退行指针（回退会出现linePtr =  -1）
				if (ch >= 32 && ch <= 126)linePtr--;
				symbol = 19;
			}
			printRes();
		}
		else if (ch == '!') {
			tokenBuffer += tolower(ch);
			ch = getch();
			if (ch == '=') {
				tokenBuffer += tolower(ch);
				symbol = 21;
				printRes();
			}
			else {
				//如果ch是换行符，则不需要回退行指针（回退会出现linePtr =  -1）
				if (ch >= 32 && ch <= 126)linePtr--;
				symbol = 21;
				//result(2);
				printerror(1);
			}
		}
		else if (ch == '=') {
			tokenBuffer += tolower(ch);
			ch = getch();
			if (ch == '=') {
				tokenBuffer += tolower(ch);
				symbol = 22;
			}
			else {
				//如果ch是换行符，则不需要回退行指针（回退会出现linePtr =  -1）
				if (ch >= 32 && ch <= 126)linePtr--;
				symbol = 33;
			}
			printRes();
		}
		//-A-字符
		else if (ch == '\'') {
			ch = getch();
			if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || isalnum(ch) || ch == '_')
				tokenBuffer += tolower(ch);
			else {
				//如果ch是换行符，则不需要回退行指针（回退会出现linePtr =  -1）
				if (ch >= 32 && ch <= 126) linePtr--;
				printerror(2);
			}
			ch = getch();
			if (ch != '\'') {
				printerror(2);
				//如果ch是换行符，则不需要回退行指针（回退会出现linePtr =  -1）
				if (ch >= 32 && ch <= 126) linePtr--;
			}
			symbol = 38; //容错，将第一个字符当做读到的字符，后面的字符继续按文法读取
			printRes();
		}
		//-A-字符串
		else if (ch == '"') {
			while (true) {
				ch = getch();
				//读到”或者\n，结束字符串的读入
				if (ch == '"' || ch == '\n') break;
				else if (ch == 32 || ch == 33 || (ch >= 35 && ch <= 126)) tokenBuffer += tolower(ch);
				else printerror(3);
			}
			if (ch != '"') printerror(4);
			symbol = 37; //容错
			printRes();
		}
		//单分节符
		else if (ch == '+') {
			tokenBuffer += tolower(ch);
			symbol = 13;
			printRes();
		}
		else if (ch == '-') {
			tokenBuffer += tolower(ch);
			symbol = 14;
			printRes();
		}
		else if (ch == '*') {
			tokenBuffer += tolower(ch);
			symbol = 15;
			printRes();
		}
		else if (ch == '/') {
			tokenBuffer += tolower(ch);
			symbol = 16;
			printRes();
		}
		else if (ch == '&') {
			tokenBuffer += tolower(ch);
			symbol = 41;
			printRes();
		}
		else if (ch == '|') {
			tokenBuffer += tolower(ch);
			symbol = 42;
			printRes();
		}
		else if (ch == '~') {
			tokenBuffer += tolower(ch);
			symbol = 43;
			printRes();
		}
		else if (ch == '(') {
			tokenBuffer += tolower(ch);
			symbol = 25;
			printRes();
		}
		else if (ch == ')') {
			tokenBuffer += tolower(ch);
			symbol = 26;
			printRes();
		}
		else if (ch == '[') {
			tokenBuffer += tolower(ch);
			symbol = 27;
			printRes();
		}
		else if (ch == ']') {
			tokenBuffer += tolower(ch);
			symbol = 28;
			printRes();
		}
		else if (ch == '{') {
			tokenBuffer += tolower(ch);
			symbol = 29;
			printRes();
		}
		else if (ch == '}') {
			tokenBuffer += tolower(ch);
			symbol = 30;
			printRes();
		}
		else if (ch == ',') {
			tokenBuffer += tolower(ch);
			symbol = 31;
			printRes();
		}
		else if (ch == ';') {
			tokenBuffer += tolower(ch);
			symbol = 32;
			printRes();
		}
		else if (ch == '.') {
			tokenBuffer += tolower(ch);
			symbol = 40;
			printRes();
		}
		else printerror(1);
	}
	else return false;
	return true;
}
