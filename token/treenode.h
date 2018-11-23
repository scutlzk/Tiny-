#pragma once
#include"Token.h"
typedef  string NodeType;
typedef  string ValType;
typedef  string ObjType;

class TreeNode
{
public:
	NodeType    nodetype;     // 节点类型
	ValType     valtype;	    //  值类型: "TK_INT" "TK_STRING" "TK_BOOL"
	TreeNode    *child[3];	    // 子节点的指针
	Token        tk;		   // 当节点是FACTOR类型时该成员才有效
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
	Token tk;          // 符号(词法分析器的返回值)
	ObjType objtype;   // 符号对象类型：函数fun/变量var/常量const
	Kind valtype;   // 值类型: "TK_INT" "TK_STRING" "TK_BOOL"
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

