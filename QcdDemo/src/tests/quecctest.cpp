#include "quecctest.h"
#include <iostream>
#include <vector>
#include <cmath>

#include <qcd_api.h>
#include <qcd_debug.h>

#include "benchmark.h"






void QuEccTest::run()
{
    std::cout << "\n>..QuEccTest: begin" << std::endl;
    Benchmark_Begin(quecc_test);

    //undone yet

    std::cout << "\n>..QuEccTest: end" << std::endl;
    Benchmark_End(quecc_test);

}
