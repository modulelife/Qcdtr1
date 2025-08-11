#define _USE_MATH_DEFINES
#include "grovertest.h"
#include <iostream>
#include <cmath>
#include <complex>
#include <bitset>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>

#include <qcd_api.h>
#include <qcd_debug.h>

#include "benchmark.h"




static std::string toBitsetString(qcd_ullong state, int bitwidth) {
    std::string result;
    for (int i = 0; i < bitwidth; i++) {
        result = ((state & (1ULL << i)) ? '1' : '0') + result;
    }
    return result;
}


static void addMultiCZ(qcd_uint target_qubit,
    const std::vector<qcd_uint>& control_qubits) {
    if (control_qubits.empty()) {
        QcDebug(qcdGate(QCD_Z, target_qubit));
    }
    else {
        std::vector<qcd_bool> control_values(control_qubits.size(), QCD_TRUE);

        QcDebug(qcdMultiControlGate(QCD_Z, target_qubit,
            control_qubits.data(),
            control_values.data(),
            control_qubits.size()));
    }
}




void GroverTest::run() {
    std::cout << "\n>..GroverTest: begin" << std::endl;
    Benchmark_Begin(grover_test);


    const qcd_uint max_qubits = qcdQubitCapacity();
    std::cout << ">>damn, " << max_qubits << " qubits at most\n";

    int num_qubits = 0;
    do {
        std::cout << ">>set the working bitwidth: ";
        std::cin >> num_qubits;
        if (num_qubits < 1 || num_qubits > max_qubits) {
            std::cout << ">>Invalid input! Please enter between 1 and " << max_qubits <<"\n";
        }
    } while (num_qubits < 1 || num_qubits > max_qubits);


    qcd_ullong target_state = 0;
    bool valid_input = false;
    while (!valid_input) {
        std::cout << ">>Enter target state: ";
        std::cin >> target_state;

        if (target_state >= (1ULL << num_qubits)) {
            std::cout << ">>Range Error: input must between 0 and "
                << (1ULL << num_qubits) - 1 << "\n";
            continue;
        }

        valid_input = true;
    }

    std::cout << ">..Configuration: " << num_qubits << " qubits | Target: |"
        << toBitsetString(target_state, num_qubits) << "> (decimal: " << target_state << ")\n";

    

    QcDebug(qcdNewCircuit(num_qubits));

    
    qcd_uint seg_init;
    QcDebug(qcdNewSegment(&seg_init));
    for (qcd_uint i = 0; i < num_qubits; i++) {
        QcDebug(qcdGate(QCD_H, i));
    }
    QcDebug(qcdSubmitSegment(QCD_TRUE));



    qcd_uint seg_loop;
    QcDebug(qcdNewSegment(&seg_loop));

    // Oracle: 
    for (qcd_uint i = 0; i < num_qubits; i++) {
        if (!(target_state & (1ULL << i))) {
            QcDebug(qcdGate(QCD_X, i));
        }
    }
    // flip phase
    if (num_qubits > 1) {
        std::vector<qcd_uint> controls;
        for (qcd_uint i = 0; i < num_qubits - 1; i++) {
            controls.push_back(i);
        }
        addMultiCZ(num_qubits - 1, controls);
    }
    else {
        QcDebug(qcdGate(QCD_Z, 0));
    }

    // recover initial X
    for (qcd_uint i = 0; i < num_qubits; i++) {
        if (!(target_state & (1ULL << i))) {
            QcDebug(qcdGate(QCD_X, i));
        }
    }

    // spread
    for (qcd_uint i = 0; i < num_qubits; i++) {
        QcDebug(qcdGate(QCD_H, i));
        QcDebug(qcdGate(QCD_X, i));
    }
    // multi-CZ
    if (num_qubits > 1) {
        std::vector<qcd_uint> controls;
        for (qcd_uint i = 0; i < num_qubits - 1; i++) {
            controls.push_back(i);
        }
        addMultiCZ(num_qubits - 1, controls);
    }
    else {
        QcDebug(qcdGate(QCD_Z, 0));
    }
    for (qcd_uint i = 0; i < num_qubits; i++) {
        QcDebug(qcdGate(QCD_X, i));
        QcDebug(qcdGate(QCD_H, i));
    }
    QcDebug(qcdSubmitSegment(QCD_TRUE));





    const double n = pow(2, num_qubits);
    const int iterations = static_cast<int>(M_PI / 4.0 * sqrt(n));
    std::cout << ">> Optimal iterations: " << iterations << std::endl;

    QcDebug(qcdZeroStateInit());
    QcDebug(qcdExecuteSegment(seg_init));

    
    const qcd_ullong dim = 1ULL << num_qubits;
    double max_prob_final = 0.0;
    qcd_ullong max_state_final = 0;

    // Grover loop
    for (int iter = 0; iter < iterations; iter++) {
        QcDebug(qcdExecuteSegment(seg_loop));

        double max_prob = 0.0;
        qcd_ullong max_state = 0;
        std::cout << "\n>> Measurement results (probability > 1%):" << std::endl;
        for (qcd_ullong i = 0; i < dim; i++) {
            double prob = qcdGetBaseProbability(i);
            if (prob > max_prob) {
                max_prob = prob;
                max_state = i;
            }

            if (prob > 0.01) {
                std::cout << "  |" << toBitsetString(i, num_qubits)
                    << ">: " << std::fixed << std::setprecision(2)
                    << (prob * 100) << "%" << std::endl;
            }
        }
        if (iter == iterations - 1) {
            max_prob_final = max_prob;
            max_state_final = max_state;
        }
    }


    std::cout << "\n>> Most probable solution: |"
        << toBitsetString(max_state_final, num_qubits)
        << "> (" << std::fixed << std::setprecision(1)
        << (max_prob_final * 100) << "%)" << std::endl;

    if (max_state_final == target_state) {
        std::cout << ">> SUCCESS: Found target state!" << std::endl;
    }
    else {
        std::cout << ">> FAILURE: Expected |"
            << toBitsetString(target_state, num_qubits)
            << "> but found |"
            << toBitsetString(max_state_final, num_qubits)
            << ">" << std::endl;
    }

    QcDebug(qcdClearBuffer());


    std::cout << "\n>..GroverTest: end" << std::endl;
    Benchmark_End(grover_test);
}