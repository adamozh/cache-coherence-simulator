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

enum ProcessorState { FREE, LOAD, STORE, NON_MEMORY, MEM_ACCESS };

class ProcessorImpl : public Processor {
  private:
    vector<pair<unsigned int, unsigned int>> stream;
    shared_ptr<Protocol> protocol;
    shared_ptr<Cache> l1Data;
    shared_ptr<Bus> bus;

    ProcessorState state = FREE;
    shared_ptr<Request> currRequest = nullptr;
    unsigned int cycles = 0;

    unsigned int memCounter = 0;
    unsigned int nonMemCounter = 0;

    bool done = false;

    void execute(unsigned int type, unsigned int value);

  public:
    ProcessorImpl(string filepath, unsigned int cacheSize, unsigned int associativity,
                  unsigned int blockSize, shared_ptr<Bus> bus, shared_ptr<Protocol> protocol);
    void executeCycle();
    void invalidateCache();
    bool isDone();
};
