#define _USE_MATH_DEFINES
#include "grovertest.h"
#include <iostream>
#include <cmath>
#include <complex>
#include <bitset>

#include <qcd_api.h>
#include <qcd_debug.h>

#include "benchmark.h"






void GroverTest::run() {
    std::cout << "\n>..GroverTest: begin" << std::endl;
    Benchmark_Begin(grover_test);

    //undone yet

    std::cout << "\n>..GroverTest: end" << std::endl;
    Benchmark_End(grover_test);
}