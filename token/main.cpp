
#include"four_yuan.h"
using namespace std;
int main()
{
	//deal_text("test1.txt");
	deal_text("test2.txt");

	auto root = program();
	prod_code(root);
	for (auto a : code)
		cout << a << endl;
	system("pause");
}

