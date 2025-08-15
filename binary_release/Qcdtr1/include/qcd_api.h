/*****************************************LICENSE*****************************************
* 
*		COMMERCIAL LICENSE WITH THIRD - PARTY MIT COMPONENT
*		
*		Copyright(c) 2025 Cipher - All Rights Reserved
*		
*		This software and associated documentation files(the "Software") are
*		proprietary products owned exclusively by Cipher.
*		
*		-- - GRANT OF RIGHTS-- -
*		NON - COMMERCIAL USE IS EXPRESSLY PERMITTED WITHOUT RESTRICTION.
*		Users may freely use, copy, and distribute the Software for non - commercial
*		purposes including :
*		*Personal / educational use
*		*Academic research
*		*Non - profit projects
*		*Open source development
*		
*		COMMERCIAL USE, DISTRIBUTION, OR MODIFICATION REQUIRES EXPLICIT WRITTEN 
*		PERMISSION FROM CIPHER.
*		To obtain commercial licensing terms, contact: https://github.com/modulelife
*		
*		--- THIRD - PARTY COMPONENT NOTICE-- -
*		The Software incorporates a modified version of Qulacs library :
*		-Original work : Copyright(c) 2018 Qulacs Authors
*		- License : MIT License(full text below)
*		- Modifications : Rebuilt and packaged as internal API implementation
*		- Component usage : This library functions as internal computational
*		engine with no exposed interfaces
*		
*		Users of this Software are granted rights ONLY for the Qulacs component
*		under the following MIT License terms :
*		
*		============================================== =
*		MIT License
*		
*		Copyright(c) 2018 Qulacs Authors
*		
*		Permission is hereby granted, free of charge, to any person obtaining a copy
*		of this software and associated documentation files(the "Software"), to deal
*		in the Software without restriction, including without limitation the rights
*		to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
*		copies of the Software, and to permit persons to whom the Software is
*		furnished to do so, subject to the following conditions :
*		
*		The above copyright notice and this permission notice shall be included in all
*		copies or substantial portions of the Software.
*		
*		THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*		IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*		FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
*		AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*		LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*		OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*		SOFTWARE.
*		============================================== =
*		
*		-- - ADDITIONAL RESTRICTIONS-- -
*		1. Reverse engineering, decompilation, or disassembly of any binaries
*		except the Qulacs component is strictly prohibited
*		2. Commercial use constitutes acceptance of commercial licensing terms
*		3. Non - commercial users exempt from commercial restrictions
*		4. Removal of this license file automatically voids all usage rights
*		
*		-- - DEFINITIONS-- -
*		*NON - COMMERCIAL USE : Any use not intended for commercial advantage or
*		monetary compensation
*		*COMMERCIAL USE : Any use in connection with revenue generation, business
*		operations, or professional services
* 
******************************************************************************************
*/

/***************************************** USAGE *****************************************
*
*		====================================================
*		** Basic program structure -- thread session mode **
*		====================================================
*		...
*		qcdNewCircuit(qubit_count);					// create a working context
*		
*		qcd_uint seg_id{};							// start editing a new circuit
*		qcdNewSegment(&segid);						// segment
*		...											// add gates maybe noise
*		qcdSubmitSegment(whether_optimize)			// cannot modify once submitted
* 
*		qcdxxxInit(...);							// initialize quantum state
*		qcdExecuteSegment(seg_id);					// this may take long
* 
*		qcdGetBaseProbability(base_id);				// getting results
* 
*		qcdClearBuffer();							// clear the quantum state buffer
* 
* 
*		========================================================
*		** Stack-like editing -- note the order of submission **
*		========================================================
* 
*		.-----qcdNewSegment(&seg1);
*		|     ...
*		| .---qcdNewSegment(&seg2);
*		| |   ...
*		| | .--qcdNewSegment(&seg3);
*		| | | ...
*		| | *--qcdSubmitSegment(QCD_TRUE);
*		| | .--qcdNewSegment(&seg4);
*		| | | ...
*		| | *--qcdSubmitSegment(QCD_TRUE);
*		| *----qcdSubmitSegment(QCD_TRUE);
*		*------qcdSubmitSegment(QCD_TRUE);
* 
*		qcdNewSegment and qcdSubmitSegment matches like parentheses: ((()()))
* 
*
******************************************************************************************
*/

#ifndef QCD_API_H
#define QCD_API_H

#include <qcd_enum.h>

// Platform compatibility
#ifdef QCD_STATIC
#define _QCD_API
#else
#if defined(_MSC_VER)
#ifdef QCD_EXPORTS
#define _QCD_API __declspec(dllexport)
#else
#define _QCD_API __declspec(dllimport)
#endif
#define _QCD_ALIGN8 __declspec(align(8))
#else
#define _QCD_API __attribute__((visibility("default")))
#define _QCD_ALIGN8 __attribute__((aligned(8)))
#endif
#endif


// API version
#define QCD_API_VERSION_MAJOR 0
#define QCD_API_VERSION_MINOR 3
#define QCD_API_VERSION_PATCH 5




#ifdef __cplusplus
#define _BEGIN_QCD_EXTERN extern "C" {
#define _END_QCD_EXTERN   }
#else
#define _BEGIN_QCD_EXTERN
#define _END_QCD_EXTERN
#endif




_BEGIN_QCD_EXTERN



typedef unsigned long long                              qcd_ullong;
typedef unsigned int                                    qcd_uint;
typedef long long                                       qcd_llong;
typedef int                                             qcd_int;
typedef int                                             qcd_bool;
typedef int                                             qcd_enum;

#if defined(_MSC_VER)
#pragma pack(push, 8)
#elif defined(__GNUC__) || defined(__clang__)
#pragma pack(push)
#pragma pack(8)
#endif
typedef _QCD_ALIGN8 struct { 
	double real;
	double imag;
}														qcd_complex;
#pragma pack(pop)


// =================================
// Quantum Simulation System Setup
// =================================

/**
 * Query the max number of qubits available to hardware memory
 *
 * @return max qubit number
 */
_QCD_API qcd_uint qcdQubitCapacity();

/**
 * Release the memory of quantum state if there is any
 *
 */
_QCD_API void qcdClearBuffer();


// =================================
// Quantum Circuit Construction
// =================================

/**
 * Start a new Circuit.
 * Caution!! Old circuit(if there is any) would be deleted
 *
 * @param Circ_width: The number of qubits to use
 */
_QCD_API void qcdNewCircuit(qcd_uint Circ_width);

/**
 * Get the qubit width of the current circuit
 *
 * @return qubit width of the current circuit, 0 means no current circuit
 */
_QCD_API qcd_uint qcdCurrentCircuitWidth();

/**
 * Start editing a new circuit segment
 *
 * @param Segment_handle: [out] return the handle of new segment, 0 means fail
 */
_QCD_API void qcdNewSegment(qcd_uint* Segment_handle);

/**
 * Finish editing this circuit segment
 *
 * @param Enable_optimizer: QCD_TRUE to optimize the circuit ( maybe a bit faster in
 * execution)
 */
_QCD_API void qcdSubmitSegment(qcd_bool Enable_optimizer);

/**
 * Delete a circuit segment
 *
 * @param Segment_handle: the handle of the segment
 */
_QCD_API void qcdDeleteSegment(qcd_uint Segment_handle);

/**
 * Add a basic quantum gate to a single qubit
 *
 * @param Gate_type: The type of gate to apply
 * @param Target_qubit: The target qubit index
 */
_QCD_API void qcdGate(qcd_enum Gate_type, qcd_uint Target_qubit);

/**
 * Add a controlled quantum gate
 *
 * @param Gate_type: The type of gate to apply
 * @param Target_qubit: The target qubit index
 * @param Control_qubit: The control qubit index
 * @param Control_value: True control or false control
 */
_QCD_API void qcdControlGate(qcd_enum Gate_type, qcd_uint Target_qubit,
	qcd_uint Control_qubit, qcd_bool Control_value);

/**
 * Add a multi-controlled quantum gate
 *
 * @param Gate_type: The type of gate to apply
 * @param Target_qubit: The target qubit index
 * @param Control_qubits: The control qubit indices
 * @param Control_values: List of control values
 * @param Ctrl_list_len: Number of control qubits
 */
_QCD_API void qcdMultiControlGate(qcd_enum Gate_type, qcd_uint Target_qubit,
	const qcd_uint* Control_qubits, const qcd_bool* Control_values,
	qcd_uint Ctrl_list_len);

/**
 * Add a rotation gate with a specific Angle
 *
 * @param Gate_type: Rotation gate type (RX, RY, RZ or their inverse)
 * @param Target_qubit: The target qubit index
 * @param Angle: Rotation angle in radians
 */
_QCD_API void qcdRotationGate(qcd_enum Gate_type, qcd_uint Target_qubit,
	double Angle);

/**
 * Add a controlled rotation gate with a specific Angle
 *
 * @param Gate_type: Rotation gate type (RX, RY, RZ or their inverse)
 * @param Target_qubit: The target qubit index
 * @param Angle: Rotation angle in radians
 * @param Control_qubit: The control qubit index
 * @param Control_value: True control or false control
 */
_QCD_API void qcdControlRotationGate(qcd_enum Gate_type, qcd_uint Target_qubit,
	double Angle, qcd_uint Control_qubit, qcd_bool Control_value);

/**
 * Add a multi-controlled rotation gate with a specific Angle
 *
 * @param Gate_type: Rotation gate type (RX, RY, RZ or their inverse)
 * @param Target_qubit: The target qubit index
 * @param Angle: Rotation angle in radians
 * @param Control_qubits: The control qubit indices
 * @param Control_values: List of control values
 * @param Ctrl_list_len: Number of control qubits
 */
_QCD_API void qcdMultiControlRotationGate(qcd_enum Gate_type,
	qcd_uint Target_qubit, double Angle, const qcd_uint* Control_qubits,
	const qcd_bool* Control_values, qcd_uint Ctrl_list_len);

/**
 * Add a swap gate to a pair of qubits
 *
 * @param Target_qubit1: One target qubit index
 * @param Target_qubit2: The other target qubit index
 */
_QCD_API void qcdSwapGate(qcd_uint Target_qubit1, qcd_uint Target_qubit2);

/**
 * Add a CNOT gate to a pair of qubits
 *
 * @param Target_qubit: The target qubit index
 * @param Control_qubit: The control qubit index
 */
_QCD_API void qcdCNOTGate(qcd_uint Target_qubit, qcd_uint Control_qubit);

/**
 * Add a CZ gate to a pair of qubits
 *
 * @param Target_qubit: The target qubit index
 * @param Control_qubit: The control qubit index
 */
_QCD_API void qcdCZGate(qcd_uint Target_qubit, qcd_uint Control_qubit);

/**
 * Add a Toffoli gate to a triplet of qubits
 *
 * @param Target_qubit: The target qubit index
 * @param Control_qubit1: The control qubit1 index
 * @param Control_qubit2: The control qubit2 index
 */
_QCD_API void qcdToffoliGate(qcd_uint Target_qubit, qcd_uint Control_qubit1,
	qcd_uint Control_qubit2);

/**
* Add a U1 gate with a specific parameter
*
* @param Target_qubit: The target qubit index
* @param Lambda
*/
_QCD_API void qcdU1Gate(qcd_uint Target_qubit, double Lambda);

/**
* Add a U2 gate with specific parameters
*
* @param Target_qubit: The target qubit index
* @param Phi
* @param Lambda
*/
_QCD_API void qcdU2Gate(qcd_uint Target_qubit, double Phi, double Lambda);

/**
* Add a U3 gate with specific parameters
*
* @param Target_qubit: The target qubit index
* @param Theta
* @param Phi
* @param Lambda
*/
_QCD_API void qcdU3Gate(qcd_uint Target_qubit, double Theta, double Phi,
	double Lambda);

/**
* Add a controlled U1 gate with a specific parameter
*
* @param Target_qubit: The target qubit index
* @param Lambda
* @param Control_qubit: The control qubit index
* @param Control_value: True control or false control
*/
_QCD_API void qcdControlU1Gate(qcd_uint Target_qubit, double Lambda,
	qcd_uint Control_qubit, qcd_bool Control_value);

/**
* Add a controlled U2 gate with specific parameters
*
* @param Target_qubit: The target qubit index
* @param Phi
* @param Lambda
* @param Control_qubit: The control qubit index
* @param Control_value: True control or false control
*/
_QCD_API void qcdControlU2Gate(qcd_uint Target_qubit, double Phi, double Lambda,
	qcd_uint Control_qubit, qcd_bool Control_value);

/**
* Add a controlled U3 gate with specific parameters
*
* @param Target_qubit: The target qubit index
* @param Theta
* @param Phi
* @param Lambda
* @param Control_qubit: The control qubit index
 *@param Control_value: True control or false control
*/
_QCD_API void qcdControlU3Gate(qcd_uint Target_qubit, double Theta, double Phi,
	double Lambda, qcd_uint Control_qubit, qcd_bool Control_value);

/**
* Add a controlled U1 gate with a specific parameter
*
* @param Target_qubit: The target qubit index
* @param Lambda
* @param Control_qubits: The control qubit indices
* @param Control_values: List of control values
* @param Ctrl_list_len: Number of control qubits
*/
_QCD_API void qcdMultiControlU1Gate(qcd_uint Target_qubit, double Lambda,
	const qcd_uint* Control_qubits, const qcd_bool* Control_values,
	qcd_uint Ctrl_list_len);

/**
* Add a controlled U2 gate with a specific parameter
*
* @param Target_qubit: The target qubit index
* @param Phi
* @param Lambda
* @param Control_qubits: The control qubit indices
* @param Control_values: List of control values
* @param Ctrl_list_len: Number of control qubits
*/
_QCD_API void qcdMultiControlU2Gate(qcd_uint Target_qubit, double Phi,
	double Lambda, const qcd_uint* Control_qubits, const qcd_bool* Control_values,
	qcd_uint Ctrl_list_len);

/**
* Add a controlled U3 gate with a specific parameter
*
* @param Target_qubit: The target qubit index
* @param Theta
* @param Phi
* @param Lambda
* @param Control_qubits: The control qubit indices
 *@param Control_values: List of control values
 *@param Ctrl_list_len: Number of control qubits
*/
_QCD_API void qcdMultiControlU3Gate(qcd_uint Target_qubit, double Theta,
	double Phi, double Lambda, const qcd_uint* Control_qubits,
	const qcd_bool* Control_values, qcd_uint Ctrl_list_len);

/**
 * Add noise to a single qubit
 *
 * @param Noise_type: The type of noise to apply
 * @param Target_qubit: The target qubit index
 * @param Probability: 
 */
_QCD_API void qcdNoise(qcd_enum Noise_type, qcd_uint Target_qubit, double Probability);

/**
 * Add measurement to a single qubit
 *
 * @param Target_qubit: The target qubit index
 * @param Classical_reg_addr: The classical register to save result
 */
_QCD_API void qcdMeasurement(qcd_uint Target_qubit, qcd_uint Classical_reg_addr);


// =================================
// Simulation Execution
// =================================

/**
 * Initialize quantum state to |00..0>
 *
 */
_QCD_API void qcdZeroStateInit();

/**
 * Initialize quantum state to a Haar random state
 *
 */
_QCD_API void qcdHaarRandomInit();

/**
 * Initialize quantum state to any one of the comptational basis
 *
 * @param Bitset: taking the lower n bits, LSB corresponds to qubit[0]
 */
_QCD_API void qcdBitsetInit(qcd_ullong Bitset);

/**
 * Apply a circuit segment to the current quantum state
 *
 * @param Segment_handle: The segment to execute
 */
_QCD_API void qcdExecuteSegment(qcd_uint Segment_handle);


// =================================
// Getting results
// =================================

/**
 * Get the dimension of statespace
 *
 * @return the dimension of statespace
 */
_QCD_API qcd_ullong qcdGetStateDim();

/**
 * Get the probability of a specific qubit being in the |0> state
 *
 * @param Qubit_index: The qubit to measure
 * @return the probability
 */
_QCD_API double qcdGetQubitProbabilityZero(qcd_uint Qubit_index);


/**
 * Get the probability of a specific computational basis state
 *
 * @param Base_vector_index: The index of a certain computational base
 * @return the probability
 */
_QCD_API double qcdGetBaseProbability(qcd_ullong Base_vector_index);

/**
 * Get the projection on a specific computational base vector
 *
 * @param Base_vector_index: the index of a certain computational base
 * @return the complex amplitude
 */
_QCD_API qcd_complex qcdGetBaseComponent(qcd_ullong Base_vector_index);

/**
 * Get the information entropy of current quantum state on computational basis
 *
 * @return the entropy of state, unit: [bit]
 */
_QCD_API double qcdGetStateEntropy();

/**
 * Get the marginal probability of a subset of qubit configuration
 *
 * @param Measure_qubits: The subset of qubits to measure
 * @param Measure_values: The expected measurement result
 * @param Measure_list_len: Number of qubits to measure
 * @return the probability
 */
_QCD_API double qcdGetMarginalProbability(const qcd_uint* Measure_qubits,
	const qcd_bool* Measure_values, qcd_uint Measure_list_len);

/**
 * Get the value of a classical register
 *
 * @param Classical_reg_addr: Which classical register
 * @return the value
 */
_QCD_API qcd_uint qcdGetClassicalValue(qcd_uint Classical_reg_addr);


// =================================
// Error Handling
// =================================

/**
 * Get the last error code for the current thread
 *
 * @return error code
 */
_QCD_API qcd_enum qcdGetError();

/**
 * Get the error discription of corresponding error code
 *
 * @param Error_code: valid error code
 * @return error discription
 */
_QCD_API const char* qcdWhatError(qcd_enum Error_code);

/**
 * Clear the error state for the current thread
 */
_QCD_API void qcdClearError();

_END_QCD_EXTERN

#endif // QCD_API_H