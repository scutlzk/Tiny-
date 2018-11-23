#include"Token.h"
#include<fstream>
#include<cctype>
vector<Token*> text;
map<string, Kind> string_to_Kind;
void init_string_to_Kind(string file = "kind.txt")
{
	fstream f;
	f.open(file);
	if (!f)cerr << "f.open(kind.txt);" << endl;
	int n;
	f >> n;
	string a, b;
	for (int i = 0;i < n;++i) {
		f >> a >> b;
		string_to_Kind.insert(pair<string, Kind>(b, a));
	}
}
void deal_text(string file)
{
	init_string_to_Kind();
	fstream f;
	f.open(file);
	if (!f)cerr << "deal_text f.open(file);" << endl;
	char c;
	int line = 1;
	int Tokennum = 1;
	string *tok = new string;
	int errno_t = 0;
	string m_errno;
	while (f.peek() != EOF)
	{
		errno_t = 0;
		m_errno = "";
		f.get(c);
		if (c == '\n')
		{
			line++;
			Tokennum = 1;
			continue;
		}
		if (c == '\'')
		{
			while (f.get(c) && c != '\''&&c != '\n')
				*tok += c;
			if (c == '\'')
				text.push_back(new Token("STR", line, Tokennum++, *tok));
			if (c == '\n')
			{
				text.push_back(new Token("STR", line, Tokennum++, *tok,0,1, "引号无对齐"));
				f.putback(c);
			}
			tok = new string;
			continue;
		}
		if (c == '{')
		{
			while (f.get(c) && c != '}'&&c != EOF)
			{
				if (c == '\n') { line++; Tokennum = 1;}
			}
			if (c == '}') { Tokennum++; }
			if (c == EOF)
			{
				break;
			}
			continue;
		}
		if (c == '}')
		{
			for (auto it = text.begin();it != text.end();)
			{
				if ((*it)->line == line)
					it = text.erase(it);    //删除元素，返回值指向已删除元素的下一个位置    
				else
					++it;    //指向下一个位置
			}
			tok = new string;
			continue;
		}
		if (isalpha(c))
		{

			*tok = c;
			while (f.get(c) && isalnum(c))
			{
				*tok += c;
			}
			if (string_to_Kind.find(*tok) != string_to_Kind.end())
			{
				text.push_back(new Token(string_to_Kind[*tok],line, Tokennum++,"KEY"));
			}
			else
			{
				text.push_back(new Token("ID",line, Tokennum++,*tok));
			}
			f.putback(c);
			tok = new string;
			continue;
		}
		if (isdigit(c))
		{
			*tok = c;
			while (f.get(c) && isdigit(c))
			{
				*tok += c;
			}
			if (isalpha(c))
			{
				*tok += c;
				while (f.get(c) && isalnum(c))
				{
					*tok += c;
				}
				text.push_back(new Token("ID", line, Tokennum++, *tok, 0,1, "非法标识符"));
			}
			else
				text.push_back(new Token("NUM", line, Tokennum++, *tok,stoi(*tok)));
			f.putback(c);
			tok = new string;
			continue;
		}
		if (c == '>' || c == '<' || c == ':')
		{
			*tok = c;
			f.get(c);
			if (c == '=')
				*tok += c;
			else
			{
				f.putback(c);
			}
			text.push_back(new Token(string_to_Kind[*tok], line, Tokennum++,"SYM"));
			tok = new string;
			continue;

		}
		if (c == ' ' || c == '\t')

			continue;
		*tok = c;
		if (string_to_Kind.find(*tok) != string_to_Kind.end())
			text.push_back(new Token(string_to_Kind[*tok], line, Tokennum++, "SYM"));
		tok = new string;
	}
}
void ff()
{
	for (auto q : text)
	{
		if (q->error_t)
			cout << q->m_error << endl;
		else
			cout << q->kind << " " << q->sval << " " << q->ival << endl;;
		
	}
}