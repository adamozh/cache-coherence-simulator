#include "bus.hpp"
#include "bus_impl.hpp"
#include "bus_impl_dragon.hpp"
#include "bus_impl_moesi.hpp"
#include "cache.hpp"
#include "cache_line.hpp"
#include "cache_set.hpp"
#include "dragon.hpp"
#include "memory.cpp"
#include "mesi.hpp"
#include "moesi.hpp"
#include "processor.hpp"
#include "processor_impl.hpp"
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

using namespace std;

bool debug = false;

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
    if (protocol != "MESI" && protocol != "Dragon" && protocol != "MOESI") {
        cerr << "Invalid protocol. Choose either MESI, MOESI or Dragon." << endl;
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
    // ensure that command can accept other protocols
    shared_ptr<Bus> bus;
    vector<shared_ptr<Processor>> processors;
    shared_ptr<Protocol> protocolPtr;
    if (protocol == "MESI") {
        bus = make_shared<BusImpl>(blockSize);
        protocolPtr = make_shared<MESIProtocol>(); // update this to initialise other protocols
    } else if (protocol == "Dragon") {
        bus = make_shared<BusImplDragon>(blockSize, associativity, cacheSize);
        protocolPtr = make_shared<DragonProtocol>(); // update this to initialise other protocols
    } else if (protocol == "MOESI") {
        bus = make_shared<BusImplMOESI>(blockSize);
        protocolPtr = make_shared<MOESIProtocol>();
    } else {
        cout << "protocol does not exist yet will sometime in the future! :)" << endl;
        return 1;
    }
    cout << "loading files" << endl;
    int pid = 0;
    for (const auto &entry : filesystem::directory_iterator(folderPath)) {
        string filepath = entry.path().string();
        shared_ptr<Processor> processor = make_shared<ProcessorImpl>(
            pid, filepath, cacheSize, associativity, blockSize, bus, protocolPtr);
        if (debug) cout << "current bus" << (bus == nullptr) << endl;
        bus->attachProcessor(processor);
        cout << "attached processor to bus" << endl;
        processors.push_back(processor);
        pid++;
        cout << "loaded files for " << filepath << endl;
    }
    cout << "starting execution:" << endl;
    unsigned int clock = 0;
    while (true) {
        if (debug) cout << "========== CLOCK CYCLE: " << clock << " ==========" << endl;
        for (int i = 0; i < processors.size(); i++) {
            if (processors[i]->isDone() == false) processors[i]->executeCycle();
        }
        bus->executeCycle();
        bool isDone = all_of(processors.begin(), processors.end(),
                             [&](shared_ptr<Processor> p) { return p->isDone(); });

        if (debug) {
            for (int i = 0; i < processors.size(); i++) {
                processors[i]->printProgressInline();
            }
            cout << endl;
            bus->printProgress();
        }

        if (isDone) {
            break;
        } else {
            clock++;
        }
        // if (debug && clock == 1000) break;
    }
    /*
     * Simulation Statistics
     */
    cout << "=========== SIMULATION STATISTICS ===========" << endl;
    for (auto p : processors) {
        p->printStatistics();
    }
    bus->printStatistics();
    cout << "Shared accesses: " << bus->getNumShared() + protocolPtr->getNumShared() << endl;
    cout << "Private accesses: " << bus->getNumPrivate() + protocolPtr->getNumPrivate() << endl;
    //compute cycles per core
    unsigned int totalComputeCycles = 0;
    //load instructions per core
    unsigned int totalLoadInstructions = 0;
    //store instructions per core
    unsigned int totalStoreInstructions = 0;
    //idle cycles per core
    unsigned int totalIdleCycles = 0;
    for (auto p: processors){
        totalComputeCycles += p->getNumComputeCycles();
        totalLoadInstructions += p->getNumLoad();
        totalStoreInstructions += p->getNumStore();
        totalIdleCycles += p->getNumIdle();
    }
    cout << "Number of compute cycles per core is: " << fixed << setprecision(2) << (double)totalComputeCycles/(double)4 << endl;
    cout << "Number of load instructions per core is: " << fixed << setprecision(2) << (double)totalLoadInstructions/(double)4 << endl;
    cout << "Number of store instructions per core is: " << fixed << setprecision(2) << (double)totalStoreInstructions/(double)4 << endl;
    cout << "Number of idle cycles per core is: " << fixed << setprecision(2) << (double)totalIdleCycles/(double)4 << endl;

    return 0;
}
