#include "quecctest.h"
#include <iostream>
#include <vector>
#include <cmath>

#include <qcd_api.h>
#include <qcd_debug.h>

#include "benchmark.h"





static void encodeShorCode(qcd_uint data_qubit,
    const std::vector<qcd_uint>& ancilla_qubits) {
    QcDebug(qcdCNOTGate(ancilla_qubits[0], data_qubit));
    QcDebug(qcdCNOTGate(ancilla_qubits[1], data_qubit));

    QcDebug(qcdGate(QCD_H, data_qubit));
    QcDebug(qcdGate(QCD_H, ancilla_qubits[0]));
    QcDebug(qcdGate(QCD_H, ancilla_qubits[1]));

    QcDebug(qcdCNOTGate(ancilla_qubits[3], data_qubit));
    QcDebug(qcdCNOTGate(ancilla_qubits[4], ancilla_qubits[0]));
    QcDebug(qcdCNOTGate(ancilla_qubits[5], ancilla_qubits[1]));
}

static void measureSyndrome(qcd_uint data_qubit,
    const std::vector<qcd_uint>& ancilla_qubits,
    qcd_uint syndrome_qubit) {
    QcDebug(qcdCNOTGate(syndrome_qubit, data_qubit));
    QcDebug(qcdCNOTGate(syndrome_qubit, ancilla_qubits[0]));
    QcDebug(qcdCNOTGate(syndrome_qubit, ancilla_qubits[1]));

    QcDebug(qcdCNOTGate(syndrome_qubit, data_qubit));
    QcDebug(qcdCNOTGate(syndrome_qubit, ancilla_qubits[3]));
    QcDebug(qcdCNOTGate(syndrome_qubit, ancilla_qubits[4]));
}

static void correctErrors(qcd_uint data_qubit,
    const std::vector<qcd_uint>& ancilla_qubits,
    qcd_uint syndrome_qubit) {
    qcd_uint flip_controls[1] = { syndrome_qubit };
    qcd_bool flip_values[1] = { QCD_TRUE };

    QcDebug(qcdMultiControlGate(QCD_X, data_qubit, flip_controls, flip_values, 1));
    QcDebug(qcdMultiControlGate(QCD_X, ancilla_qubits[0], flip_controls, flip_values, 1));
    QcDebug(qcdMultiControlGate(QCD_X, ancilla_qubits[1], flip_controls, flip_values, 1));

    QcDebug(qcdMultiControlGate(QCD_Z, data_qubit, flip_controls, flip_values, 1));
    QcDebug(qcdMultiControlGate(QCD_Z, ancilla_qubits[3], flip_controls, flip_values, 1));
    QcDebug(qcdMultiControlGate(QCD_Z, ancilla_qubits[4], flip_controls, flip_values, 1));
}



constexpr int NUM_DATA_QUBITS = 4;
constexpr int ANCILLA_PER_DATA = 5;
constexpr int SYNDROME_QUBITS = 4;
constexpr int TOTAL_QUBITS = NUM_DATA_QUBITS * (1 + ANCILLA_PER_DATA) + SYNDROME_QUBITS;

void QuEccTest::run()
{
    std::cout << "\n>..QuEccTest: begin" << std::endl;
    Benchmark_Begin(quecc_test);


    
    double NOISE_PROB{};
    do {
        std::cout << ">>set error rate: ";
        std::cin >> NOISE_PROB;
        if (NOISE_PROB < 0.0 || NOISE_PROB > 1.0) {
            std::cout << ">>Invalid input! Please enter between 0 and 1\n";
        }
    } while (NOISE_PROB < 0.0 || NOISE_PROB > 1.0);

    std::cout << ">> Total qubits: " << TOTAL_QUBITS
        << " (4 data + " << (TOTAL_QUBITS - NUM_DATA_QUBITS) << " ancilla)"
        << std::endl;

    
    QcDebug(qcdNewCircuit(TOTAL_QUBITS));

    // ================== seg1 sender ==================
    qcd_uint encode_seg;
    QcDebug(qcdNewSegment(&encode_seg));
    {
        for (int i = 0; i < NUM_DATA_QUBITS; i++) {
            qcd_uint data_q = i;
            QcDebug(qcdGate(QCD_H, data_q));

            std::vector<qcd_uint> ancilla;
            for (int j = 0; j < ANCILLA_PER_DATA; j++) {
                ancilla.push_back(NUM_DATA_QUBITS + i * ANCILLA_PER_DATA + j);
            }

            encodeShorCode(data_q, ancilla);
        }
    }
    QcDebug(qcdSubmitSegment(QCD_TRUE));
    std::cout << ">> Segment 1: Encoding submitted" << std::endl;

    // ================== seg2 noisy channel ==================
    qcd_uint noise_seg;
    QcDebug(qcdNewSegment(&noise_seg));
    {
        for (qcd_uint q = 0; q < TOTAL_QUBITS; q++) {
            QcDebug(qcdNoise(QCD_BIT_FLIP, q, NOISE_PROB));
            QcDebug(qcdNoise(QCD_DEPHASING, q, NOISE_PROB / 2));
        }
    }
    QcDebug(qcdSubmitSegment(QCD_TRUE));
    std::cout << ">> Segment 2: Noise added (p=" << NOISE_PROB << ")" << std::endl;

    // ================== seg3 receiver ==================
    qcd_uint correct_seg;
    QcDebug(qcdNewSegment(&correct_seg));
    {
        for (int i = 0; i < NUM_DATA_QUBITS; i++) {
            qcd_uint data_q = i;
            std::vector<qcd_uint> ancilla;
            for (int j = 0; j < ANCILLA_PER_DATA; j++) {
                ancilla.push_back(NUM_DATA_QUBITS + i * ANCILLA_PER_DATA + j);
            }

            qcd_uint syndrome_q = TOTAL_QUBITS - SYNDROME_QUBITS + i;

            measureSyndrome(data_q, ancilla, syndrome_q);
            correctErrors(data_q, ancilla, syndrome_q);
        }
    }
    QcDebug(qcdSubmitSegment(QCD_TRUE));
    std::cout << ">> Segment 3: Error correction submitted" << std::endl;

    // ================== execution & analyse ==================
    QcDebug(qcdZeroStateInit());

    std::cout << ">> Executing quantum circuit..." << std::endl;
    Benchmark_Begin(circ_execution);
    QcDebug(qcdExecuteSegment(encode_seg));
    QcDebug(qcdExecuteSegment(noise_seg));
    QcDebug(qcdExecuteSegment(correct_seg));
    Benchmark_End(circ_execution);

    
    double avg_fidelity = 0.0;
    for (int i = 0; i < NUM_DATA_QUBITS; i++) {
        double prob0 = qcdGetQubitProbabilityZero(i);
        double fidelity = 1.0 - 2.0 * std::abs(prob0 - 0.5);
        avg_fidelity += fidelity;

        std::cout.precision(4);
        std::cout << ">> Data qubit " << i << " fidelity: " << fidelity * 100.0 << "%\n";
    }
    avg_fidelity /= NUM_DATA_QUBITS;

    std::cout << ">> Average fidelity: " << avg_fidelity * 100.0 << "%\n";
    std::cout << ">> Unprotected error rate: " << (1.0 - avg_fidelity) * 100.0 << "%\n";

    if (avg_fidelity > 0.85) {
        std::cout << ">> SUCCESS: Quantum error correction effective" << std::endl;
    }
    else {
        std::cout << ">> WARNING: Error rate exceeds threshold" << std::endl;
    }

    QcDebug(qcdClearBuffer());


    std::cout << "\n>..QuEccTest: end" << std::endl;
    Benchmark_End(quecc_test);
}