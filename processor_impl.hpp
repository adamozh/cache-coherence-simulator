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

    unsigned int memCounter = 0;
    unsigned int nonMemCounter = 0;

    bool done = false;

    bool execute(unsigned int type, unsigned int value);

  public:
    ProcessorImpl(int pid, string filepath, unsigned int cacheSize, unsigned int associativity,
                  unsigned int blockSize, shared_ptr<Bus> bus, shared_ptr<Protocol> protocol);
    void executeCycle() override;
    void invalidateCache() override;
    State getState(unsigned int address) override;
    bool isDone() override;
    void setState(unsigned int address, State state) override;
    int getPID() override { return pid; };
    void printProgressInline() override;
};
