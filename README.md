# Cache Coherence Simulator
A trace-driven cache coherence simulator for invalidation-based MESI, MOESI protocols, and update-based Dragon Protocol, implemented in C++. The protocols are evaluated based on their execution on various PARSEC benchmarks suites, namely blackscholes, bodytrack, and fluidanimate.

## PARSEC
Princeton Application Repository for Shared-Memory Computers (PARSEC) is a publicly available benchmark suite developed by Princeton University, in collaboration with Intel. It comprises a set of workloads that simulate real-world programs utilizing multicore processors, and therefore it is used here to evaluate the effectiveness of our cache coherence protocols in different application domains. 3 data-parallel benchmarks were picked: blacksholes simuates a financial analysis workload, bodytrack simulates a computer vision workload, while fluidanimate simulates an animation workload. Their traces can be found in their respective folders.

## Assumptions
1. The simulation runs sequentially. In 1 simulation cycle, first each processor executes a cycle, then the bus executes a cycle.
1. Bus operations are sequential, however all memory accesses happen in parallel, meaning within one cycle, all current memory accesses are processed together.
1. Flush operations triggered by BusRd or BusRdX requests are processed immediately, and are treated as part of the request itself.
1. For MOESI, if a processor has the required cache block in M, and O state, it can send the cache block to other processors.
1. For Dragon, locking of the cache line of other processors when the Sm cache is modified will take 1 cycle.
1. On load and on store functions modifies 1 word per instruction.


## Running the simulator
Build the `coherence` program with the provided Makefile
```
make
```
The coherence program takes input in the following format: `Usage: ./coherence <protocol> <path-to-benchmark-trace> <cache-size> <associativity> <block-size>`
`cache-size` and `block-size` are in bytes, and this assumes a word size of 4 bytes.

For example, the following would run a simulation of the bodytrack benchmark on the MESI protocol, with a cache size of 1024 bytes, a set associativity of 2, and a block size of 16 bytes.
```
./coherence MESI bodytrack 1024 2 16
```
In this case, there would be 1024/(2*16) = 32 sets, each set would have 2 blocks, and each block would be 16 bytes = 4 words.

## Acknowledgements
This was originally developed as an assignment for CS4223.
