/**
* @file: main.cpp
*
*
*/
#include <iostream>

#include "tests/qpetest.h"
#include "tests/grovertest.h"





int main()
{
	std::unique_ptr<Test> up_test;
	unsigned op;
	std::cout << "\n\tchoose a test:\n\t\t1.Quantum phase estimation\n\t\t2.Grover search\n\n\tenter: ";
	std::cin >> op;

	switch (op) {
	case 1:
		up_test = std::make_unique<QpeTest>();
		break;
	case 2:
		up_test = std::make_unique<GroverTest>();
		break;
	default:
		up_test = std::make_unique<Test>();
		break;
	}

	up_test->run();

	system("pause");
	return 0;
}