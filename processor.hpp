#pragma once

#include "cache.hpp"
#include "protocol.hpp"
#include "request.hpp"
#include <memory>
#include <vector>

using namespace std;

enum ProcessorState { FREE, LOAD, STORE, NON_MEMORY, MEM_ACCESS };

class Processor {
  private:
    shared_ptr<Protocol> protocol;
    shared_ptr<Cache> l1Data;
    shared_ptr<Bus> bus;

    ProcessorState state;
    shared_ptr<Request> currRequest;
    unsigned int cycles = 0;

    unsigned int memCounter = 0;
    unsigned int nonMemCounter = 0;

    void execute(unsigned int type, unsigned int value);
    void issueBusTransaction();

  public:
    Processor(unsigned int cacheSize, unsigned int associativity, unsigned int blockSize,
              shared_ptr<Bus> bus, shared_ptr<Protocol> protocol);
    void executeCycle();
    void invalidateCache();
};
