#ifndef QCD_ENUM_H
#define QCD_ENUM_H


#define QCD_H											10000
#define QCD_NOT											10001
#define QCD_X											10001
#define QCD_Y											10002
#define QCD_Z											10003
#define QCD_sqrtX										10004
#define QCD_sqrtXdag									10005
#define QCD_sqrtY										10006
#define QCD_sqrtYdag									10007
#define QCD_S											10008
#define QCD_T											10009
#define QCD_Sdag										10010
#define QCD_Tdag										10011
#define QCD_P0											10012
#define QCD_P1											10013
#define QCD_RX											10014
#define QCD_RY											10015
#define QCD_RZ											10016
#define QCD_InvRX										10017
#define QCD_InvRY										10018
#define QCD_InvRZ										10019

#define QCD_BIT_FLIP									10100
#define QCD_DEPHASING									10101
#define QCD_DEPOLARIZING								10103
#define QCD_INDEPENDENT_XZ								10104
#define QCD_AMPLITUDE_DAMPING							10105


#define QCD_NO_ERROR									20000
// the context of this thread is damaged
#define QCD_INVALID_CONTEXT								20001
// usually qubit index out of range
#define QCD_INVALID_QUBIT								20002
// 1.the segment to execute is not submitted or deleted or seg_handle is wrong
// 2.no segment being edited
#define QCD_SEGMENT_NOT_EXIST							20003
// the gate type is not supported in current interface
#define QCD_INVALID_GATE_TYPE							20004
// call qcdxxxInit first
#define QCD_NOT_INITIALIZED								20005
// call qcdNewCircuit first
#define QCD_NOT_IMPLEMENTED								20006
// things like Circ_width, Control_value, Probability is not reasonable
#define QCD_INVALID_PARAMETER							20007
// you have passed in a nullptr somewhere, check control_qubits and control_values
#define QCD_NULL_POINTER								20008
// unknown faults that spoil the execution
#define QCD_EXECUTION_FAILURE							20009
// 1.the quantum state buffer has been deleted
// 2.call qcdxxxInit first
#define QCD_RESULT_UNAVAILABLE							20010
// the computational basis index out of range
#define QCD_INVALID_BASE_INDEX							20011
// call qcdQubitCapacity to see how many qubits this computer can handle
#define QCD_OUT_OF_MEMORY								20012
// the noise type is not supported in current interface
#define QCD_INVALID_NOISE_TYPE							20013
// a qubit controls itself
#define QCD_CONTROL_TARGET_COINCIDE						20014
// 1.SWAP gate can only act on two different qubits
// 2.the same qubit shows up several times in qubit list
#define QCD_DUPLICATED_QUBITS							20015


#define QCD_TRUE  1
#define QCD_FALSE 0


#endif