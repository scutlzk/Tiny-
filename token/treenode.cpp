#include"treenode.h"
#include<set>
Token token;
set<string> symname;
set< Sym* > symtable;
vector<error_m> error_ms;
Sym* insertSym(string name)
{
	auto q = symname.insert(name);
	Sym* w = nullptr;
	if (q.second)
	{
		w = new Sym;
		symtable.insert(w);
	}
	return w;
}
Sym* findSym(Token token_)
{
	for (auto q : symtable)
	{
		if (q->tk.sval == token_.sval)
		{
			return q;
		}
	}
	return nullptr;
}
int len = 0;
void GetToken() {
	if (len < text.size())
		token = *text[len++];
	else
		token = Token();
}
void GetToken_p()
{
	if (len > 0)
		token = *text[----len];
	else
		token = Token();
}

void GetnextLine()
{
	int a = token.line;
	while (token.line == a)
		GetToken();
}
bool match(vector<Kind> a, int is_e = 0,string message="") {
	
	for (auto p : a)
	{
		if (p == token.kind)
		{
			GetToken();
			return true;
		}
	}
	if(is_e)
		error_ms.push_back(error_m(token.line, message));
	return false;
}
void declarations() {
	GetToken();
declarations1:
	while (token.kind == "TK_INT" || token.kind == "TK_BOOL" || token.kind == "TK_STRING") {
		Token typet_ = token;//符“int,bool,string”
									// 跳过类型说明
		do {
			GetToken();
			Token id = token;		     // 保存标识符的符号
			if (match({ "ID" },1,"期望ID")) {
				Sym *sym = insertSym(id.sval); // 插入符号表
				// 填写符号的各种属性
				if (!sym)
				{
					error_ms.push_back(error_m(id.line, "重复声明"));
					GetnextLine();
					goto declarations1;
				}
				sym->tk = id;
				sym->objtype = "var";         //变量
				sym->valtype = typet_.kind;
			}
			else
			{
				GetnextLine();
				goto declarations1;
			}
		} while (token.kind == "TK_COMMA"); //逗号

		if (!match({ "TK_SEMICOLON" }, 1, "期望分号"))	       	 // 匹配分号
		{
			GetnextLine();
		}
	}
}
vector<Kind> factor_set = { "NUM","STR","ID","TK_TRUE","TK_FALSE" };
vector<Kind> add_op_set = { "TK_ADD","TK_SUB" };
vector<Kind> comparison_op_set = { "TK_LEQ","TK_GEQ","TK_LSS","TK_TRUE","TK_EQU", "TK_GTR" };
TreeNode*add_exp();
TreeNode* stmt_sequence();
TreeNode *comparision_exp() {
	TreeNode *add_exp_node = nullptr;
	add_exp_node = add_exp();
	Token prev_token = token;
	if (match(comparison_op_set)) {
		TreeNode *comparison_node=nullptr;
		if (prev_token.kind == "TK_LEQ")
			comparison_node = new TreeNode("LEQ_EXP", add_exp_node, comparision_exp());
		if (prev_token.kind == "TK_GEQ")
			comparison_node = new TreeNode("GEQ_EXP", add_exp_node, comparision_exp());
		if (prev_token.kind == "TK_LSS")
			comparison_node = new TreeNode("LSS_EXP", add_exp_node, comparision_exp());
		if (prev_token.kind == "TK_EQU")
			comparison_node = new TreeNode("EQU_EXP", add_exp_node, comparision_exp());
		if (prev_token.kind == "TK_GTR")
			comparison_node = new TreeNode("GTR_EXP", add_exp_node, comparision_exp());
		comparison_node->valtype = "TK_BOOL";
		return comparison_node;
	}
	return add_exp_node;
}

TreeNode *log_and_exp() {
	TreeNode *log_and_exp_node = nullptr;
	TreeNode *another_log_and_exp_node = nullptr;
	log_and_exp_node = comparision_exp();
	if (match({ "TK_AND" })) {
		another_log_and_exp_node = log_and_exp();
	}
	if (another_log_and_exp_node) {
		TreeNode *tmp = new TreeNode("LOG_AND_EXP", log_and_exp_node, another_log_and_exp_node);
		tmp->valtype = "TK_BOOL";
		return tmp;
	}
	else
		return log_and_exp_node;
}
TreeNode *log_or_exp() {
	TreeNode *log_and_exp_node = nullptr, *log_or_exp_node = nullptr;
	log_and_exp_node = log_and_exp();
	if (match({ "TK_OR" })) {
		log_or_exp_node = log_or_exp();
	}
	if (log_or_exp_node) {
		TreeNode *tmp = new TreeNode("LOG_OR_EXP", log_and_exp_node, log_or_exp_node);
		tmp->valtype = "TK_BOOL";
		return tmp;
	}
	else
		return log_and_exp_node;
}

TreeNode* factor() {
	TreeNode *mul_exp_node = nullptr;
	if (match({ "TK_LP" })) {
		TreeNode *log_or_exp_node = log_or_exp();
		if (!match({ "TK_RP" }, 1, "括号不匹配.")) {
			GetnextLine();
			return nullptr;
		}
		return log_or_exp_node;
	}
	Token prev_token = token;
	if (match(factor_set)) {
		TreeNode *factor_node = new TreeNode("FACTOR", prev_token);
		if (prev_token.kind == "ID") {
			if (symname.find(prev_token.sval) == symname.end())
			{
				error_ms.push_back(error_m(prev_token.line, "使用了未声明的变量."));
				GetnextLine();
				return nullptr;
			}
			else
			{
				Sym *sym = findSym(prev_token);
				factor_node->valtype = sym->valtype;
			}
		}
		if (prev_token.kind == "NUM")
			factor_node->valtype = "TK_INT";

		if (prev_token.kind == "STR")
			factor_node->valtype = "TK_STRING";

		if (prev_token.kind == "TK_TRUE" || prev_token.kind == "TK_FALSE")
			factor_node->valtype = "TK_BOOL";
		return factor_node;
	}
	else {
		error_ms.push_back(error_m(prev_token.line, "匹配数字, 字符串, 变量, true, false失败."));
		GetnextLine();
		return nullptr;
	}
}
TreeNode *mul_exp() {
	TreeNode *node1 = factor();
	if (match({ "TK_MUL" })) {
		TreeNode *node2 = mul_exp();
		if (node1->valtype != "TK_INT" || node2->valtype != "TK_INT")
		{
			error_ms.push_back(error_m(token.line, "非int不能相乘"));
			GetnextLine();
			return nullptr;
		}
		TreeNode *node3 = new TreeNode("MUL_EXP", node1, node2);
		node3->valtype = "TK_INT";
		return node3;
	}
	else {
		if (match({ "TK_DIV" })) {
			TreeNode *node2 = mul_exp();
			if (node1->valtype != "TK_INT" || node2->valtype != "TK_INT")
			{
				error_ms.push_back(error_m(token.line, "非int不能相乘"));
				GetnextLine();
				return nullptr;
			}
			TreeNode *node3 = new TreeNode("DIV_EXP", node1, node2);
			node3->valtype = "TK_INT";
			return node3;
		}
		else {
			return node1;
		}
	}
}
TreeNode *add_exp() {
	TreeNode *add_exp_node = nullptr;
	add_exp_node = mul_exp();
	if (add_exp_node->valtype != "TK_STRING" && add_exp_node->valtype != "TK_INT")
	{
		error_ms.push_back(error_m(token.line, "非数字或字符串不能进行加减乘除"));
		GetnextLine();
		return nullptr;
	}
	Token prev_token = token;
	if (match(add_op_set)) {
		TreeNode *another_add_exp_node = add_exp();
		if (add_exp_node->valtype != "TK_STRING" && add_exp_node->valtype != "TK_INT")
		{

			error_ms.push_back(error_m(token.line, "非数字或字符串不能进行加减乘除"));
			GetnextLine();
			return nullptr;
		}
		if (add_exp_node->valtype != another_add_exp_node->valtype)
		{
			error_ms.push_back(error_m(token.line, "不同类型的不能相加"));
			GetnextLine();
			return nullptr;
		}
		if (prev_token.kind == "TK_ADD") {
			TreeNode *tmp = new TreeNode("ADD_EXP", add_exp_node, another_add_exp_node);
			tmp->valtype = add_exp_node->valtype;
			return tmp;
		}
		if (prev_token.kind == "TK_SUB") {
			TreeNode *tmp = new TreeNode("SUB_EXP", add_exp_node, another_add_exp_node);
			tmp->valtype = add_exp_node->valtype;
			return tmp;
		}
	}
	return add_exp_node;
}


TreeNode* if_stmt() {
	match({ "TK_IF" }, 1, "期望是if");
	TreeNode *cond_exp = nullptr, *then_stmt = nullptr, *else_stmt = nullptr;
	cond_exp = log_or_exp();	 		// 分析条件表达式部分
	if (!match({ "TK_THEN" },1,"if 后没有then"))
	{
		GetnextLine();
		return nullptr;
	}
	then_stmt = stmt_sequence();		// 分析then部分

	if (token.kind == "TK_ELSE") {
		GetToken();
		else_stmt = stmt_sequence();	// 分析else部分
	}

	if (!match({ "TK_END" })) {
		cerr << token.kind<< endl;
		error_ms.push_back(error_m(token.line, "if控制块缺少end结束符."));
	}
	//构建IF_STMT节点，有3个孩子，分别是条件表达式、then语句和
	// else语句；若没有else语句，则第三个孩子为null
	return new TreeNode("IF_STMT", cond_exp, then_stmt, else_stmt);
}

TreeNode *assign_stmt() {
	TreeNode *node = factor();
	if (node->tk.kind != "ID")
	{
		error_ms.push_back(error_m(token.line, "赋值语句左边应该是变量."));
		GetnextLine();
		return nullptr;
	}
	if (match({ "TK_ASSIGN" })) {
		TreeNode *log_or_exp_node = log_or_exp();
		if (node->valtype != log_or_exp_node->valtype)
		{
			error_ms.push_back(error_m(token.line, "赋值语句左右两边应该是同种类型."));
			GetnextLine();
			return nullptr;

		}
		return new TreeNode("ASSIGN_STMT", node, log_or_exp_node);
	}
	else
	{
		error_ms.push_back(error_m(token.line, "赋值语句应该有:=."));
		GetnextLine();
		return nullptr;
	}
	return nullptr;
}

TreeNode *repeat_stmt() {
	if (!match({ "TK_REPEAT" }))
	{
		error_ms.push_back(error_m(token.line, "Repeat块缺少repeat关键词."));
		GetnextLine();
		return nullptr;
	}
	TreeNode *stmt_seq_node = stmt_sequence();
	if (!match({ "TK_UNTIL" }))
	{
		error_ms.push_back(error_m(token.line, "Repeat块缺少until关键词."));
		GetnextLine();
		return nullptr;
	}
	TreeNode *log_or_exp_node = log_or_exp();
	return new TreeNode("REPEAT_STMT", stmt_seq_node, log_or_exp_node);
}
TreeNode *read_stmt() {
	if (!match({ "TK_READ" }))
	{
		cerr << token.kind << endl;
		error_ms.push_back(error_m(token.line, "read块缺少read关键词."));
		GetnextLine();
		return nullptr;
	}
	TreeNode *read_node = nullptr;
	Token prev_token = token;

	if (!match({ "ID" }))
	{
		error_ms.push_back(error_m(prev_token.line, "read 后面跟变量."));
		GetnextLine();
		return nullptr;
	}
	read_node = new TreeNode("READ_STMT", prev_token);
	if (!match({ "TK_SEMICOLON" }))
	{
		error_ms.push_back(error_m(token.line, "read语句最后以';'结尾."));
		GetnextLine();
		return nullptr;
	}
	return read_node;
	// todo: how to handle ID ? as a child or store it to node.tk_
}
TreeNode *write_stmt() {
	if (!match({ "TK_WRITE" }))
	{
		error_ms.push_back(error_m(token.line, "write块缺少write关键词."));
		GetnextLine();
		return nullptr;
	}
	TreeNode *log_or_exp_node = log_or_exp();
	if (!match({ "TK_SEMICOLON" }))
	{
		error_ms.push_back(error_m(token.line, "write语句最后以';'结尾."));
		GetnextLine();
		return nullptr;
	}
	return new TreeNode("WRITE_STMT", log_or_exp_node);
}
TreeNode *while_stmt() {
	if (!match({ "TK_WHILE" }))
	{
		error_ms.push_back(error_m(token.line, "缺少while关键词."));
		GetnextLine();
		return nullptr;
	}
	TreeNode *log_or_exp_node = log_or_exp();
	if (!match({ "TK_DO" }))
	{
		error_ms.push_back(error_m(token.line, "while块缺少do关键词."));
		GetnextLine();
		return nullptr;
	}
	TreeNode *stmt_seq_node = stmt_sequence();
	if (!match({ "TK_END" }))
	{
		error_ms.push_back(error_m(token.line, "while块缺少end关键词."));
		GetnextLine();
		return nullptr;
	}
	return new TreeNode("WHILE_STMT", log_or_exp_node, stmt_seq_node);
}

TreeNode* stmt_sequence() {
	TreeNode *t1 = nullptr, *t2 = nullptr;
	// 语句开始符
	set<Kind>stmt_first = { "TK_IF","TK_WHILE","TK_REPEAT","ID","TK_READ","TK_WRITE" };
	while (stmt_first.find(token.kind)!= stmt_first.end()) {
		// 注意：语句的开始符“if、while”等已由上面的match语句匹配
		// 因此下面各语句的分析函数（if_stmt()等）不需要匹配开始符。
		if (token.kind == "TK_IF") {
			t2 = if_stmt();	goto stmt_sequence1;
		}
		if (token.kind == "TK_WHILE") {
			t2 = while_stmt(); goto stmt_sequence1;
		}
		if (token.kind == "TK_REPEAT") {
			t2 = repeat_stmt(); goto stmt_sequence1;
		}
		if (token.kind == "ID") {
			t2 = assign_stmt(); goto stmt_sequence1;
		}
		if (token.kind == "TK_READ") {

			t2 = read_stmt(); goto stmt_sequence1;
		}
		if (token.kind == "TK_WHILE") {
			t2 = write_stmt(); goto stmt_sequence1;
		}
		GetnextLine();
		continue;
	stmt_sequence1:
		// 见下页
		if (t1 == nullptr)
			t1 = t2;
		else {
			// 构建STMT_SEQUENCE节点,
			//t1,t2分别为左右孩子
			t1 = new TreeNode("STMT_SEQUENCE", t1, t2);
		}
	}
	return t1;
}
/*
if_stmt()              new TreeNode(IF_STMT, cond_exp, then_stmt, else_stmt) ;
repeat_stmt()      new TreeNode( REPEAT_STMT , rep_stmt , cond_exp ) ;
assign_stmt()      new TreeNode( ASSIGN_STMT , cond_exp ) ;
read_stmt()         new TreeNode( READ_STMT ) ;
write_stmt()         new TreeNode( WRITE_STMT , cond_exp ) ;
while_stmt()         new TreeNode( WHILE_STMT , cond_exp , rep_stmt ) ;
exp()//表达式       new TreeNode( LOG_AND/OR/NOT_EXP , t1 , t2 ) ;
comparison_exp() new TreeNode( GTR/GEQ/LSS/LEQ/EQU_EXP , t1 , t2 ) ;
arithmetic_exp()    new TreeNode( ADD/SUB_EXP , t1 , t2 ) ;
simple_exp()          new TreeNode( MUL/DIV_EXP , t1 , t2 ) ;
factor()                  new TreeNode( FACTOR ) ;
*/


TreeNode* program() {
	if (text.size() == 0)
		return nullptr;
	declarations();			// 调用declarations函数分析变量的声明


	for (auto p : symname)
		cout <<"symname"<<" "<< p<< endl;
	return stmt_sequence();	// 调用stmt_sequence()分析语句列表	
							// stmt_sequence()的返回值就是整个
							// 程序的语法树（不包括声明部分）
}
