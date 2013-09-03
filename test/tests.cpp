#include <iostream>

#include "../core/score.h"

using namespace std;
using namespace Ipponboard;

int main()
{
	try
	{
		Score::Test_Compare();
		cout << "ok" << endl;
	}
	catch (int i)
	{
		cout << i << " failed" << endl;
	}

	return 0;
}