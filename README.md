# Quantum Computing Development technical release1 (Qcdtr1)

## Overview
The QCD API is a high-performance quantum computing simulation backend designed for quantum algorithm development and experimentation. Key features include:

1.Incorporates the Qulacs C++ library as internal engine. See their homepage->[Qulacs](https://github.com/qulacs/qulacs). Supporting up to 32 qubits(requires 64GB DRAM)

2.Plug-in-and-use: Dynamic-link library design, no complex dependencies. Linux, MacOS versions are in coming.

3.Thread session mode api, no explict resource management.

## Licensing
Non-commercial use: Free (academic/personal/open-source).

Commercial use: Requires explicit permission (contact: [modulelife](https://github.com/modulelife))

## Core Concepts
### Quantum circuits
| API | usage |
|------|------|
| `qcdNewCircuit(qcd_uint Circuit_width)` | create a new circuit workbench |
| `qcdCurrentCircuitWidth()` | query the current circuit width |
| `qcdNewSegment(qcd_uint* Segment_handle)` | start editing a new segment |
| `qcdSubmitSegment(qcd_bool Enable_optimizer)` | submit current segment |
| `qcdDeleteSegment(qcd_uint Segment_handle)` | delete a segment |
| `qcd...Gate(...)` | add gates |
| `qcdNoise(qcd_enum Noise_type, qcd_uint Target_qubit, double Probability)` | add noise |
### Quantum simulation
| API | usage |
|------|------|
| `qcd...Init(...)` | initialize quantum state |
| `qcdExecuteSegment(qcd_uint Segment_handle)` | execute a segment on this quantum state|
| `qcdGetStateDim()` | get the dimension of the State Space |
| `qcdGetQubitProbabilityZero(qcd_uint Qubit_index)` | Gets the probability of a specific qubit being in the |0> state |
| `qcdGetBaseProbability(qcd_ullong Base_vector_index)` | Gets the probability of a specific computational basis state |

## Workflow
```
qcdNewCircuit(qubit_count);					// create a working context

qcd_uint seg_id{};							// start editing a new circuit
qcdNewSegment(&segid);						// segment
...											// add gates maybe noise
qcdSubmitSegment(whether_optimize);			// cannot modify once submitted

qcdxxxInit(...);							// initialize quantum state
qcdExecuteSegment(seg_id);					// this may take long

qcdGetBaseProbability(base_id);				// getting results

qcdClearBuffer();							// clear the quantum state buffer
```
