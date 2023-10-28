#include "Cache.hpp"
#include "CacheLine.hpp"
#include "CacheSet.hpp"
#include "bus.hpp"
#include "bus_impl.hpp"
#include "memory.cpp"
#include "processor.hpp"
#include <filesystem>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

/**
    Generate the following output
    1. Overall Execution Cycle (outside)  outside
    2. Number of compute cycles per core (processor)
    3. Number of load/store instruction per core (processor) -- inside processor
    4. Number of idle cycles (processor)
    5. Data cache miss rate for each core (processor)
    6. Amount of Data traffic in bytes on the bus (bus)
    7. Number of invalidations or updates on the bus (bus)
    8. Distribution of accesses to private data vs shared data
*/
// create a cacheline class
// create a cache class -- contains many cacheline class

using namespace std;

int main(int argc, char *argv[]) {
    // Check if the correct number of arguments are provided
    if (argc != 6) {
        cerr << "Usage: ./coherence <protocol> <path-to-benchmark-trace> <cache-size> "
                "<associativity> <block-size>"
             << endl;
        return 1;
    }

    // Parse the command-line arguments
    string protocol = argv[1];
    string inputFilePath = argv[2];
    int cacheSize = stoi(argv[3]);
    int associativity = stoi(argv[4]);
    int blockSize = stoi(argv[5]);

    // Validate the protocol
    if (protocol != "MESI" && protocol != "Dragon") {
        cerr << "Invalid protocol. Choose either MESI or Dragon." << endl;
        return 1;
    }

    // Validate cache size, associativity, and block size (you can add more checks)
    if (cacheSize <= 0 || associativity <= 0 || blockSize <= 0) {
        cerr << "Cache size, associativity, and block size must be greater than 0." << endl;
        return 1;
    }

    // Display the parsed information (for debugging)
    cout << "Protocol: " << protocol << endl;
    cout << "Input File Path: " << inputFilePath << endl;
    cout << "Cache Size: " << cacheSize << endl;
    cout << "Associativity: " << associativity << endl;
    cout << "Block Size: " << blockSize << endl;

    // Read folder of benchmark name
    filesystem::path folderPath(inputFilePath);
    if (!filesystem::exists(folderPath)) {
        cout << "folder does not exist" << endl;
        return 1;
    }
    if (!filesystem::is_directory(folderPath)) {
        cout << "path is not a directory" << endl;
        return 1;
    }
    for (const auto &entry : filesystem::directory_iterator(folderPath)) {
        cout << entry.path().string() << endl;
    }

    int numProcessors;
    BusImpl bus = BusImpl();
    // TODO: Initialize processors with bus
    for (int i = 0; i < numProcessors; i++) {
    }

    unsigned int clock = 0;
    /*
    while (true) {
        for each
        processor:
            p.executeCycle()
    }
    */

    return 0;
}
