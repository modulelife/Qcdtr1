#define _USE_MATH_DEFINES
#include "qpetest.h"
#include <iostream>
#include <vector>
#include <complex>

#include <qcd_api.h>
#include <qcd_debug.h>

#include "benchmark.h"


static void addInverseQft(const std::vector<int>& qubits)
{
    int n = qubits.size();
    for (int i = 0; i < n; i++) {
        QcDebug(qcdGate(QCD_H, qubits[i]));

        for (int j = i + 1; j < n; j++) {
            double angle = -M_PI / pow(2, j - i);
            QcDebug(qcdControlU1Gate(qubits[j], angle, qubits[i], QCD_TRUE));
        }
    }

    for (int i = 0; i < n / 2; i++) {
        QcDebug(qcdSwapGate(qubits[i], qubits[n - 1 - i]));
    }
}



constexpr int nq_a = 1;
constexpr double theta = .63397;



void QpeTest::run()
{
    using namespace std::complex_literals;

    std::cout << "\n>..Qcdtr1Test: begin" << std::endl;
    Benchmark_Begin(qcdtr1_test);

    qcd_uint max_qubits = qcdQubitCapacity() - nq_a;
    std::cout << ">>damn, " << max_qubits << " qubits at most\n";

    int nq_c{};
    do {
        std::cout << ">>set the working bitwidth: ";
        std::cin >> nq_c;
        if (nq_c < 1 || nq_c > max_qubits) {
            std::cout << ">>Invalid input! Please enter between 1 and " << max_qubits << "\n";
        }
    } while (nq_c < 1 || nq_c > max_qubits);
    const int nq = nq_c + nq_a;

    QcDebug(qcdNewCircuit(nq));

    qcd_uint seg_id{};
    QcDebug(qcdNewSegment(&seg_id));


    std::cout << ">> QPE on nq_c = " << nq_c << " counting qubits, nq_a = " << nq_a << " ancilla qubits\n";
    std::cout << ">> The sequence of applied gates is:\n";
    for (int i = 0; i < nq_c; ++i) {
        QcDebug(qcdGate(QCD_H, i));
        std::cout << "H" << i << " ";
    }

    QcDebug(qcdGate(QCD_X, nq_c));
    std::cout << "X" << nq_c << std::endl;

    std::complex<double> phase = std::exp(2.0 * M_PI * 1i * theta);

    int powerU = 1;
    for (int i = 0; i < nq_c; ++i) {
        int control_index = nq_c - i - 1;
        int target_index = nq_c;

        std::complex<double> phase_n = phase;
        for (int j = 0; j < i; ++j) {
            phase_n = phase_n * phase_n;
        }

        QcDebug(qcdControlU1Gate(target_index, std::arg(phase_n), control_index, QCD_TRUE));

        std::cout << "CU(" << control_index << ", " << target_index << ")^"
            << powerU << std::endl;

        powerU *= 2;
    }

    std::vector<int> counting_qubits;
    for (int i = 0; i < nq_c; ++i) {
        counting_qubits.push_back(i);
    }

    std::cout << "Inverse QFT on qubits: ";
    for (int i = 0; i < nq_c; ++i) {
        std::cout << counting_qubits[i] << " ";
    }
    std::cout << std::endl;

    addInverseQft(counting_qubits);

    QcDebug(qcdSubmitSegment(QCD_TRUE));
    QcDebug(qcdZeroStateInit());

    Benchmark_Begin(circ_execution);
    QcDebug(qcdExecuteSegment(seg_id));
    Benchmark_End(circ_execution);

    qcd_ullong max_index = 0;
    QcDebug(const qcd_ullong dim = qcdGetStateDim());
    double max_prob = 0.0;

    for (qcd_ullong i = 0; i < dim; ++i) {
        double prob{};
        QcDebug(prob = qcdGetBaseProbability(i));
        if (prob > max_prob) {
            max_prob = prob;
            max_index = i;
        }
    }

    qcd_ullong mask = (1ULL << nq_c) - 1;
    qcd_ullong count_bits = max_index & mask;

    std::cout << ">> Measurement result [q0 q1 ... ]: ";
    for (int i = 0; i < nq_c; ++i) {
        std::cout << ((count_bits >> i) & 1) << " ";
    }
    std::cout << std::endl;

    qcd_ullong reversed = 0;
    for (int i = 0; i < nq_c; i++) {
        reversed = (reversed << 1) | ((count_bits >> i) & 1);
    }
    double theta_e = static_cast<double>(reversed) / (1ULL << nq_c);

    std::cout << ">> Input theta = " << theta << std::endl;
    std::cout << ">> Estimated theta = " << theta_e << std::endl;
    std::cout << ">> Norm difference: " << std::abs(theta_e - theta) << std::endl;

    QcDebug(qcdClearBuffer());


    std::cout << "\n>..Qcdtr1Test: end" << std::endl;
    Benchmark_End(qcdtr1_test);
}