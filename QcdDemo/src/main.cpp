/**
* @file: main.cpp
*
*
*/
#include <iostream>

#include "tests/qpetest.h"
#include "tests/grovertest.h"
#include "tests/shortest.h"
#include "tests/quecctest.h"





int main()
{
	std::unique_ptr<Test> up_test;
	unsigned op;
	std::cout << "\n\tchoose a test:\n\t\t1.Quantum phase estimation\n\t\t2.Grover search\n\t\t3.Shor decomposition\n\t\t4.Quantum error correction\n\n\tenter: ";
	std::cin >> op;

	switch (op) {
	case 1:
		up_test = std::make_unique<QpeTest>();
		break;
	case 2:
		up_test = std::make_unique<GroverTest>();
		break;
	case 3:
		up_test = std::make_unique<ShorTest>();
		break;
	case 4:
		up_test = std::make_unique<QuEccTest>();
		break;
	default:
		up_test = std::make_unique<Test>();
		break;
	}

	up_test->run();

	system("pause");
	return 0;
}