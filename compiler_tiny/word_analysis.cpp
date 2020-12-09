#include<fstream>
#include<sstream>
#include<iostream>
#include<string>
#include<vector>
#include"symbol.h"
using namespace std;


string symbolList[50]; //�����ֱ�
vector <pair<string, int>> allSymbol;

string lineBuffer; //�л�����
int linePtr; //��ָ��
int lineSize; //�г���
int indexOfLine; //�б��

string tokenBuffer; //token������
int tokenPtr; //token������ָ��
int symbol; //token����

char ch; //��ǰ�������ַ�
int num; //��ǰ����������

ifstream inFile; //Դ�����ļ�
ofstream outfile; //�ʷ���������ļ�
string filename; //Դ�ļ�

//��ʼ�������ֺ������ַ�,�����ļ�
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
	cout << "������Դ�����ļ�����" << endl;
	//cin >> filename;
	filename = "code.txt";
	inFile.open(filename);
	if (!inFile.is_open()) {
		cout << "�Ҳ��������ļ���" << endl;
		exit(0);
	}
	outfile.open("resOfWordAnalysis.txt");
	if (!outfile.is_open()) {
		cout << "�޷���������ļ���" << endl;
		exit(0);
	}
}

//��һ���ַ�
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
	//cout << "����  " << line[linePtr - 1] << endl;
	return lineBuffer[linePtr - 1];
}

//���token��׼����ȡ��һ������
void next() {
	tokenBuffer = "";
	tokenPtr = 0;
	ch = getch();
}

//�ж��Ƿ��Ѿ������ļ�
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

//�ж�token�Ƿ��ǹؼ���
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

//��ӡ����
void printerror(int op) {
	if (op == 1) cout << "#Line  " << indexOfLine << " �� ���ڲ��Ϸ����ַ� [!]" << endl;
	if (op == 2) cout << "#Line  " << indexOfLine << " �� ȱ���ҵ����� " << endl;
	if (op == 3) cout << "#Line  " << indexOfLine << " �� �ַ����д��ڲ��Ϸ��ַ� '" << ch << "'" << endl;
	if (op == 4) cout << "#Line  " << indexOfLine << " �� �ַ���ȱ��[\"] " << endl;

}

//����ʷ��������������̨���ļ���
void printRes() {
	// cout << indexOfLine << "	" << symbolList[symbol] << "		" << tokenBuffer << endl;
	outfile << indexOfLine << "	" << symbolList[symbol] << "		" << tokenBuffer << endl;
	allSymbol.push_back({ tokenBuffer, symbol });
	if (indexOfLine > 9999999) {
		cout << "����ʧ�ܣ�" << endl;
		exit(0);
	}
}

//�ʷ�����
bool getsym() {
	next();
	//���Ե��ʼ�Ŀո��Ʊ�������з�
	while (ch == ' ' || ch == '	' || ch == '\n') {
		ch = getch();
		continue;
	}
	if (!isEND()) {
		//-A-������,��ʶ��
		if (isalpha(ch) || ch == '_') {
			do {
				tokenBuffer += tolower(ch);
				ch = getch();
			} while (isalnum(ch) || ch == '_');
			//���ch�ǻ��з�������Ҫ������ָ�루���˻����linePtr =  -1��
			if (ch >= 32 && ch <= 126)linePtr--;
			symbol = isKEY();
			printRes();
		}
		//-A-�޷�������
		else if (isdigit(ch)) {
			num = 0;
			do {
				num = num * 10 + (ch - '0');
				tokenBuffer += tolower(ch);
				ch = getch();
			} while (isdigit(ch));
			//���ch�ǻ��з�������Ҫ������ָ�루���˻����linePtr =  -1��
			if (ch >= 32 && ch <= 126)linePtr--;
			symbol = 35;
			printRes();
		}
		//-A-˫�ַ��ֽ�� < > ! =
		else if (ch == '<') {
			tokenBuffer += tolower(ch);
			ch = getch();
			if (ch == '=') {
				tokenBuffer += tolower(ch);
				symbol = 18;
			}
			else {
				//���ch�ǻ��з�������Ҫ������ָ�루���˻����linePtr =  -1��
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
				//���ch�ǻ��з�������Ҫ������ָ�루���˻����linePtr =  -1��
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
				//���ch�ǻ��з�������Ҫ������ָ�루���˻����linePtr =  -1��
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
				//���ch�ǻ��з�������Ҫ������ָ�루���˻����linePtr =  -1��
				if (ch >= 32 && ch <= 126)linePtr--;
				symbol = 33;
			}
			printRes();
		}
		//-A-�ַ�
		else if (ch == '\'') {
			ch = getch();
			if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || isalnum(ch) || ch == '_')
				tokenBuffer += tolower(ch);
			else {
				//���ch�ǻ��з�������Ҫ������ָ�루���˻����linePtr =  -1��
				if (ch >= 32 && ch <= 126) linePtr--;
				printerror(2);
			}
			ch = getch();
			if (ch != '\'') {
				printerror(2);
				//���ch�ǻ��з�������Ҫ������ָ�루���˻����linePtr =  -1��
				if (ch >= 32 && ch <= 126) linePtr--;
			}
			symbol = 38; //�ݴ�����һ���ַ������������ַ���������ַ��������ķ���ȡ
			printRes();
		}
		//-A-�ַ���
		else if (ch == '"') {
			while (true) {
				ch = getch();
				//����������\n�������ַ����Ķ���
				if (ch == '"' || ch == '\n') break;
				else if (ch == 32 || ch == 33 || (ch >= 35 && ch <= 126)) tokenBuffer += tolower(ch);
				else printerror(3);
			}
			if (ch != '"') printerror(4);
			symbol = 37; //�ݴ�
			printRes();
		}
		//���ֽڷ�
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
