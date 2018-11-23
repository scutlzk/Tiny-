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
	Kind kind;		// ���ŵ�Kindֵ����TK_IF��
	int ival;		// �����������ͳ�������ival��¼��				             // ������ֵ
	string sval;	        	// �������Ǳ�ʶ�����ַ�����������				        	// sval��¼��ʶ�������ֻ��ַ���
	int line;//�к�
	int col;//�к�
	int error_t;//�Ƿ�ֵ
	string m_error;//�Ƿ���Ϣ
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


