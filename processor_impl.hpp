#pragma once

#include "bus.hpp"
#include "cache.hpp"
#include "processor.hpp"
#include "protocol.hpp"
#include "request.hpp"
#include <memory>
#include <utility>
#include <vector>

using namespace std;

enum ProcessorState { FREE, LOAD, STORE, NON_MEMORY, DONE };

class ProcessorImpl : public Processor {
  private:
    string stringOfState();

  public:
    int pid;

    vector<pair<unsigned int, unsigned int>> stream;
    int streamIndex = 0;
    shared_ptr<Protocol> protocol;
    shared_ptr<Cache> cache;
    shared_ptr<Bus> bus;

    ProcessorState state = FREE;
    unsigned int nonMemCounter = 0;

    bool done = false;

    // counters for statistics
    unsigned int numCycles = 0;
    unsigned int numComputeCycles = 0;
    unsigned int numLoad = 0;
    unsigned int numStore = 0;
    unsigned int numIdle = 0;
    unsigned int numMiss = 0;

    bool execute(unsigned int type, unsigned int value);

  public:
    ProcessorImpl(int pid, string filepath, unsigned int cacheSize, unsigned int associativity,
                  unsigned int blockSize, shared_ptr<Bus> bus, shared_ptr<Protocol> protocol);
    void executeCycle() override;
    bool invalidateCache(unsigned int address) override;
    State getState(unsigned int address) override;
    bool isDone() override;
    void setState(unsigned int address, State state) override;
    void addCacheLine(unsigned int address, State state) override;
    int getPID() override { return pid; };

    void printProgressInline() override;
    void printStatistics() override;
};
