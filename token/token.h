#pragma once
#include<iostream>
#include<string>
#include<map>
#include<vector>

using namespace std;
void deal_text(string file);
typedef string Kind;

class Token
{
public:
	Kind kind;		// 符号的Kind值，如TK_IF等
	int ival;		// 若符号是整型常量，则ival记录该				             // 常量的值
	string sval;	        	// 若符号是标识符或字符串常量，则				        	// sval记录标识符的名字或字符串
	int line;//行号
	int col;//列号
	int error_t;//非法值
	string m_error;//非法信息
	Token(){}
	Token(Kind kind_,int line_,int col_,string sval_="",int ival_=0,int error_t_=0,string m_error_="")
	{
		kind = kind_;
		line = line_;
		col = col_;
		sval = sval_;
		ival = ival_;
		error_t = error_t_;
		m_error = m_error_;
	}
};
extern vector<Token*> text;
extern map<string, Kind> string_to_Kind;


