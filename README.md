# Cache Coherence Simulator
A trace-driven cache coherence simulator for invalidation-based MESI, MOESI protocols, and update-based Dragon Protocol, implemented in C++. The protocols are evaluated based on their execution on various PARSEC benchmarks suites, namely blackscholes, bodytrack, and fluidanimate.

## PARSEC
Princeton Application Repository for Shared-Memory Computers (PARSEC) is a publicly available benchmark suite developed by Princeton University, in collaboration with Intel. It comprises a set of workloads that simulate real-world programs utilizing multicore processors, and therefore it is used here to evaluate the effectiveness of our cache coherence protocols in different application domains. 3 data-parallel benchmarks were picked: blacksholes simuates a financial analysis workload, bodytrack simulates a computer vision workload, while fluidanimate simulates an animation workload. Their traces can be found in their respective folders.

## Acknowledgements
This was originally developed as an assignment for CS4223.
