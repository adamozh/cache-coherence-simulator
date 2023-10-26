#include "processor.hpp"
#include "protocol.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

using namespace std;

Processor::Processor(unsigned int cacheSize, unsigned int associativity, unsigned int blockSize,
                     shared_ptr<Bus> bus, shared_ptr<Protocol> protocol)
    : state(FREE), protocol(protocol), bus(bus) {
    l1Data = make_shared<Cache>(cacheSize, associativity, blockSize);
}

void Processor::executeCycle() {
    // TODO: read instructions line by line here
    switch (state) {
    case FREE:
        // call execute(); with instruction type and value
        break;
    case STORE:
        if (currRequest->isDone) {
            state = FREE;
        }
        cycles++;
        break;
    case LOAD:
        if (currRequest->isDone) {
            state = FREE;
        }
        cycles++;
        break;
    case NON_MEMORY:
        if (!nonMemCounter) {
            state = FREE;
        }
        cycles++;
        break;
    }
}

void Processor::execute(unsigned int type, unsigned int value) {
    switch (type) {
    case 0: // load
        protocol->onLoad(value, bus, l1Data);
        break;
    case 1: // store
        protocol->onStore(value, bus, l1Data);
        state = STORE;
    case 2: // non-memory instructions
        state = NON_MEMORY;
        nonMemCounter = value;
        break;
    default:
        throw logic_error("invalid instruction type");
    }
}

void Processor::issueBusTransaction() {}

void Processor::invalidateCache() {}
