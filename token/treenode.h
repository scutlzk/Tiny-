#pragma once
#include"Token.h"
typedef  string NodeType;
typedef  string ValType;
typedef  string ObjType;

class TreeNode
{
public:
	NodeType    nodetype;     // �ڵ�����
	ValType     valtype;	    //  ֵ����: "TK_INT" "TK_STRING" "TK_BOOL"
	TreeNode    *child[3];	    // �ӽڵ��ָ��
	Token        tk;		   // ���ڵ���FACTOR����ʱ�ó�Ա����Ч
	TreeNode(NodeType ntype, TreeNode *t1 = nullptr, TreeNode *t2 = nullptr, TreeNode *t3 = nullptr)
	{
		nodetype = ntype;
		child[0] = t1;
		child[1] = t2;
		child[2] = t3;
	}
	TreeNode(NodeType ntype, Token token) {
		nodetype = ntype;
		tk = token;
		child[0] = nullptr;
		child[1] = nullptr;
		child[2] = nullptr;
	}

};

struct Sym {
	Token tk;          // ����(�ʷ��������ķ���ֵ)
	ObjType objtype;   // ���Ŷ������ͣ�����fun/����var/����const
	Kind valtype;   // ֵ����: "TK_INT" "TK_STRING" "TK_BOOL"
};
class error_m
{public:
	int line;
	string m;
	error_m(int line_,string m_)
	{
		line = line_;
		m = m_ ;
		cerr << line << m << endl;
	}
};

extern vector<error_m> error_ms;
 TreeNode* program();

