#include "processor_impl.hpp"
#include "protocol.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

ProcessorImpl::ProcessorImpl(int pid, string filepath, unsigned int cacheSize,
                             unsigned int associativity, unsigned int blockSize,
                             shared_ptr<Bus> bus, shared_ptr<Protocol> protocol)
    : pid(pid), protocol(protocol), bus(bus) {
    cache = make_shared<Cache>(cacheSize, associativity, blockSize);
    ifstream file(filepath); // just let runtime exception throw if fail
    string line;
    unsigned int type, value;
    int line_limit_counter = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        ss >> type;
        string s;
        ss >> s;
        if (s.starts_with("0x")) s = s.substr(2);
        stringstream(s) >> hex >> value;
        stream.push_back(make_pair(type, value));
        line_limit_counter++;
        if (line_limit_counter == 10) break;
    }
}

string ProcessorImpl::stringOfState() {
    if (state == LOAD) return "LOAD";
    if (state == STORE) return "STORE";
    if (state == NON_MEMORY) return "NON_MEMORY";
    if (state == MEM_ACCESS) return "MEM_ACCESS";
    return "FREE";
}

void ProcessorImpl::executeCycle() {
    auto pair = this->stream[streamIndex];
    unsigned int type = pair.first;
    unsigned int value = pair.second;
    switch (state) {
    case FREE:
        execute(type, value);
        streamIndex++;
        break;
    case STORE:
        if (bus->isCurrentRequestDone(pid)) {
            state = FREE;
        }
        break;
    case LOAD:
        if (bus->isCurrentRequestDone(pid)) {
            state = FREE;
        }
        break;
    case NON_MEMORY:
        nonMemCounter--;
        if (!nonMemCounter) {
            state = FREE;
        }
        break;
    case MEM_ACCESS:
        break;
    }
    cycles++;
}

void ProcessorImpl::execute(unsigned int type, unsigned int value) {
    cout << "pid " << pid << " execute " << type << " " << value << endl;
    CacheResultType cacheStatus;
    switch (type) {
    case 0: // load
        cacheStatus = protocol->onLoad(pid, value, bus, cache);
        state = cacheStatus == CACHEHIT ? FREE : LOAD;
        break;
    case 1: // store
        cacheStatus = protocol->onStore(pid, value, bus, cache);
        state = cacheStatus == CACHEHIT ? FREE : STORE;
        break;
    case 2: // non-memory instructions
        state = NON_MEMORY;
        nonMemCounter = value;
        break;
    default:
        throw logic_error("invalid instruction type");
    }
}

void ProcessorImpl::invalidateCache() {}

State ProcessorImpl::getState(unsigned int address) { return cache->getCacheLineState(address); }

bool ProcessorImpl::isDone() { return done; }

void ProcessorImpl::setState(unsigned int address, State state) {
    cache->setCacheLineState(address, state);
}

void ProcessorImpl::printProgressInline() {
    cout << "[" << streamIndex << "/" << stream.size() << "; state: " << stringOfState() << "]"
         << " ";
}
