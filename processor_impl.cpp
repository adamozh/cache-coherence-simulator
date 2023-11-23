#include "processor_impl.hpp"
#include "protocol.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

int processor_debug = false;

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
        // if (line_limit_counter == 1000) break;
    }
}

string ProcessorImpl::stringOfState() {
    if (state == LOAD) return "LOAD";
    if (state == STORE) return "STORE";
    if (state == NON_MEMORY) return "NON_MEMORY";
    if (state == DONE) return "DONE";
    return "FREE";
}

void ProcessorImpl::executeCycle() {
    numCycles++;
    auto pair = this->stream[streamIndex];
    unsigned int type = pair.first;
    unsigned int value = pair.second;
    switch (state) {
        bool isCacheNotBlocked;
    case FREE:
        isCacheNotBlocked = execute(type, value); // return boolean here
        if (isCacheNotBlocked) {
            streamIndex++;
        } else {
            numIdle++;
        }

        if (streamIndex == stream.size()) {
            done = true;
            state = DONE;
        }
        break;
    case STORE:
        numIdle++;
        if (bus->isCurrentRequestDone(pid)) {
            state = FREE;
            if (streamIndex == stream.size()) {
                done = true;
                state = DONE;
            }
        }
        break;
    case LOAD:
        numIdle++;
        if (bus->isCurrentRequestDone(pid)) {
            state = FREE;
            if (streamIndex == stream.size()) {
                done = true;
                state = DONE;
            }
        }
        break;
    case NON_MEMORY:
        numComputeCycles++;
        nonMemCounter--;
        if (!nonMemCounter) {
            state = FREE;
            if (streamIndex == stream.size()) {
                done = true;
                state = DONE;
            }
        }
        break;
    case DONE:
        break;
    }
}

bool ProcessorImpl::execute(unsigned int type, unsigned int value) {
    if (processor_debug)
        cout << "EXECUTE: PROCESSOR " << pid << " execute " << type << " " << value << endl;
    CacheResultType cacheStatus;
    switch (type) {
    case 0: // load
        cacheStatus = protocol->onLoad(pid, value, bus, cache);
        if (cacheStatus == CACHEBLOCKED) {
            return false;
        } else if (cacheStatus == CACHEMISS) {
            numMiss++;
        }
        numLoad++;
        state = cacheStatus == CACHEHIT ? FREE : LOAD;
        break;
    case 1: // store
        cacheStatus = protocol->onStore(pid, value, bus, cache);
        if (cacheStatus == CACHEBLOCKED) {
            return false;
        } else if (cacheStatus == CACHEMISS) {
            numMiss++;
        }
        numStore++;
        state = cacheStatus == CACHEHIT ? FREE : STORE;
        break;
    case 2: // non-memory instructions
        state = NON_MEMORY;
        nonMemCounter = value;
        break;
    default:
        throw logic_error("invalid instruction type");
    }
    return true;
}

bool ProcessorImpl::invalidateCache(unsigned int address) {
    return cache->invalidateCacheLine(address);
}

State ProcessorImpl::getState(unsigned int address) { return cache->getCacheLineState(address); }

bool ProcessorImpl::isDone() { return done; }

void ProcessorImpl::setState(unsigned int address, State state) {
    cache->setCacheLineState(address, state);
}

void ProcessorImpl::addCacheLine(unsigned int address, State state) {
    // TODO: this function suppose to update cache if it exist else do other things
    bool isStateSet = cache->setCacheLineState(address, state);
    if (!isStateSet) {
        // this means that cache is not inside the cacheline
        if (cache->checkCacheLineFull(address)) {
            // if address is full, then will need to invalidate cache
            // get the cacheline and then add it to the bus
            unsigned int old_cacheline_address = cache->getLRUCacheLineAddress(address);
            // put into the bus to send to cache
            // create a request if it is modified state
            State old_state = cache->getLRUCacheLineState(address);
            if ((old_state == M) || (old_state == Sm) || (old_state == O)) {
                shared_ptr<Request> flushRequest =
                    make_shared<Request>(-1, BusRd, old_cacheline_address);
                // put it into the bus
                bus->pushRequestToBus(flushRequest);
            }
        }
        cache->addCacheLine(address, state); // will automatically remove LRU cache
    }
}

void ProcessorImpl::printProgressInline() {
    cout << "[" << streamIndex << "/" << stream.size() << "; state: " << stringOfState() << "]"
         << " ";
    // usleep(100000);
}

void ProcessorImpl::printStatistics() {
    cout << "PROCESSOR " << pid << endl;
    cout << "Total cycles: " << numCycles << endl;
    cout << "Compute cycles: " << numComputeCycles << endl;
    cout << "Total idle cycles: " << numIdle << endl;

    cout << "Total load instructions: " << numLoad << endl;
    cout << "Total store instructions: " << numStore << endl;
    double missRate = (double)numMiss / (double)(numLoad + numStore);
    cout << "Miss rate: " << fixed << setprecision(3) << missRate << endl;
}
