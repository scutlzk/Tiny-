#include"four_yuan.h"
vector< string> code;
int label = 0;
int var = 1;

string  next_name() {
	return string("t") + to_string(var++);
}
string  next_label() {
	return string("L") + to_string(label++);
}
void stmt_seq_node(TreeNode *root);
void stmt_node(TreeNode *root);
void if_node(TreeNode *root, string end_label);
void while_node(TreeNode *root, string end_label);
void read_node(TreeNode *root);
void write_node(TreeNode *root);
void repeat_node(TreeNode *root, string end_label);
void assign_node(TreeNode *root);
void cond_exp(TreeNode *root, string true_label, string false_label);
string log_or(TreeNode *root);
string log_and(TreeNode *root);
string comparison_exe(TreeNode *root);
string add_exp(TreeNode *root);
string mul_exe(TreeNode *root);
string factor(TreeNode *root);
string next_label();
string next_name();
void  if_node(TreeNode *root, string end_label) {
	string before_else_label = end_label;
	string before_then_label = next_label();
	if (root->child[2]) {
		before_else_label = next_label();
		cond_exp(root->child[0], before_then_label, before_else_label);
		code.push_back("Label " + before_then_label);
		stmt_seq_node(root->child[1]);
		code.push_back("goto " + end_label);
		code.push_back("Label " + before_else_label);
		stmt_seq_node(root->child[2]);
	}
	else {
		cond_exp(root->child[0], before_then_label, end_label);
		code.push_back("Label " + before_then_label);
		stmt_seq_node(root->child[1]);
		code.push_back("Label " + end_label);
	}
}

string  log_or(TreeNode *root) {

	if (root->nodetype == "LOG_OR_EXP") {
		string log_and_res = log_and(root->child[0]);
		string another_log_or_res = log_or(root->child[1]);
		string log_or_res = next_name();
		code.push_back(log_or_res + ":=" + log_and_res + "or" + another_log_or_res);
		return log_or_res;
	}
	else
		return log_and(root);
}


string  log_and(TreeNode *root) {
	if (root->nodetype == "LOG_AND_EXP")
	{
		string comp_exp_res = comparison_exe(root->child[0]);
		string second_log_and_res = log_and(root->child[1]);
		string log_and_res = next_name();
		code.push_back(log_and_res + ":=" + comp_exp_res + "and" + second_log_and_res);
		return log_and_res;
	}
	else
		return comparison_exe(root);
}


string  comparison_exe(TreeNode *root) {
	if (root->nodetype == "LEQ_EXP" || root->nodetype == "LSS_EXP" ||
		root->nodetype == "GTR_EXP" || root->nodetype == "GEQ_EXP" || root->nodetype == "EQU_EXP")
	{
		string comp_left = add_exp(root->child[0]);
		string comp_right = comparison_exe(root->child[1]);
		string op;

		if (root->nodetype == "LEQ_EXP")
			op = "<=";
		if (root->nodetype == "LSS_EXP")
			op = "<";
		if (root->nodetype == "GTR_EXP")
			op = ">";
		if (root->nodetype == "GEQ_EXP")
			op = ">=";
		if (root->nodetype == "EQU_EXP")
			op = "=";
		return comp_left + op + comp_right;
	}
	else
		return add_exp(root);
}


string  add_exp(TreeNode *root) {
	if (root->nodetype == "ADD_EXP" || root->nodetype == "SUB_EXP")
	{
		string add_left = mul_exe(root->child[0]);
		string add_right = add_exp(root->child[1]);
		string add_res = next_name();
		string op;
		if (root->nodetype == "ADD_EXP")
			op = "+";
		if (root->nodetype == "SUB_EXP")
			op = "-";
		code.push_back(add_res + ":=" + add_left + op + add_right);
		return add_res;
	}
	return mul_exe(root);
}


string  mul_exe(TreeNode *root) {
	if (root->nodetype == "MUL_EXP" || root->nodetype == "DIV_EXP")
	{
		string mul_left = mul_exe(root->child[0]);
		string mul_right = add_exp(root->child[1]);
		string mul_res = next_name();
		string op;
		if (root->nodetype == "MUL_EXP")
			op = "*";
		if (root->nodetype == "DIV_EXP")
			op = "/";
		code.push_back(mul_res + " := " + mul_left + op + mul_right);
		return mul_res;
	}
	else
		return factor(root);
}




string  factor(TreeNode *root) {
	if (root->tk.kind == "ID")
		return root->tk.sval;
	if (root->tk.kind == "NUM")
		return root->tk.sval;
	if (root->tk.kind == "STR")
		return string("'") + root->tk.sval + "'";
	if (root->tk.kind == "TK_FALSE")
		return "false";
	if (root->tk.kind == "TK_TRUE")
		return "true";
	return log_or(root);
}


void  cond_exp(TreeNode *root, string true_label, string false_label) {

	if (root->nodetype == "LOG_OR_EXP")
	{
		string before_second_log_or_label = next_label();
		cond_exp(root->child[0], true_label, before_second_log_or_label);
		code.push_back("Label " + before_second_log_or_label);
		cond_exp(root->child[1], true_label, false_label);
		return;
	}
	if (root->nodetype == "LOG_AND_EXP")
	{
		string before_second_log_and_label = next_label();
		cond_exp(root->child[0], before_second_log_and_label, false_label);
		code.push_back("Label " + before_second_log_and_label);
		cond_exp(root->child[1], true_label, false_label);
		return;
	}

	string comp_res = comparison_exe(root);
	code.push_back("if " + comp_res + " goto " + true_label);
	code.push_back("goto " + false_label);
	return;
}


void  assign_node(TreeNode *root) {
	string log_or_res = log_or(root->child[1]);
	string id_res = factor(root->child[0]);
	code.push_back(id_res + ":=" + log_or_res);
}

void  while_node(TreeNode *root, string end_label) {
	string before_cond_label = next_label();
	string before_then_label = next_label();
	code.push_back("Label " + before_cond_label);
	cond_exp(root->child[0], before_then_label, end_label);
	code.push_back("Label " + before_then_label);
	stmt_seq_node(root->child[1]);
	code.push_back("goto " + before_cond_label);

}

void  read_node(TreeNode *root) {
	code.push_back("read " + root->tk.sval);
}

void  write_node(TreeNode *root) {
	code.push_back("write " + root->child[0]->tk.sval);
}

void  repeat_node(TreeNode *root, string end_label) {
	string before_stmt_label = next_label();
	code.push_back("Label " + before_stmt_label);
	stmt_seq_node(root->child[0]);
	cond_exp(root->child[1], end_label, before_stmt_label);
}

void  stmt_seq_node(TreeNode *root) {
	if (root->nodetype == "STMT_SEQUENCE") {
		stmt_seq_node(root->child[0]);
		stmt_node(root->child[1]);
		return;
	}
	stmt_node(root);
	return;
}

void  stmt_node(TreeNode *root) {
	if (!root)
		return;
	string after_stmt_label = next_label();
	if (root->nodetype == "IF_STMT")
		if_node(root, after_stmt_label);
	if (root->nodetype == "ASSIGN_STMT")
		assign_node(root);
	if (root->nodetype == "WHILE_STMT")
		while_node(root, after_stmt_label);
	if (root->nodetype == "REPEAT_STMT")
		repeat_node(root, after_stmt_label);
	if (root->nodetype == "READ_STMT")
		read_node(root);
	if (root->nodetype == "WRITE_STMT")
		write_node(root);
	code.push_back("Label " + after_stmt_label);
}
vector<string>  prod_code(TreeNode *root) {
	if (!root)
		return{};
	stmt_seq_node(root);
	return code;
}