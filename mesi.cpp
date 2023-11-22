#include "mesi.hpp"
#include "request.hpp"
#include "state.hpp"
#include <stdexcept>

using namespace std;

bool mesi_debug = false;

CacheResultType MESIProtocol::onLoad(int pid, unsigned int address, shared_ptr<Bus> bus,
                                     shared_ptr<Cache> cache) {
    State state = cache->getCacheLineState(address);
    // handle counters
    if (state == M || state == E) {
        numPrivate++;
    } else if (state == S) {
        numShared++;
    }

    // actual processing
    if (state == M || state == E || state == S) {
        if (mesi_debug) cout << "M/E/S: load hit" << endl;
        cache->updateCacheLine(address, state);
        return CACHEHIT;
    } else if (state == I) {
        if (mesi_debug) cout << "I: load miss, pushing BusRd" << endl;
        shared_ptr<Request> busRdRequest = make_shared<Request>(pid, BusRd, address);
        bus->pushRequestToBus(busRdRequest);
        return CACHEMISS;
    } else {
        throw runtime_error("invalid state");
    }
}

CacheResultType MESIProtocol::onStore(int pid, unsigned int address, shared_ptr<Bus> bus,
                                      shared_ptr<Cache> cache) {
    State state = cache->getCacheLineState(address);
    if (state == M) {
        // cache hit
        if (mesi_debug) cout << "M: store hit" << endl;
        numPrivate++;
        cache->updateCacheLine(address, M); // no change in state, but set most recently used
        return CACHEHIT;
    } else if (state == E) {
        // change cachestate to M and cache hit
        if (mesi_debug) cout << "E: store hit, change to M" << endl;
        numPrivate++;
        // change the state of the cache here
        cache->updateCacheLine(address, M);
        return CACHEHIT;
    } else if (state == S) {
        if (mesi_debug) cout << "S: store miss, pushing BusRdX" << endl;
        // BusRdX is only processed when popped from bus
        shared_ptr<Request> busRdXRequest = make_shared<Request>(pid, BusRdX, address);
        bus->pushRequestToBus(busRdXRequest);
        return CACHEMISS;
    } else if (state == I) {
        if (mesi_debug) cout << "I: store miss, pushing BusRdX" << endl;
        shared_ptr<Request> busRdXRequest = make_shared<Request>(pid, BusRdX, address);
        bus->pushRequestToBus(busRdXRequest);
        return CACHEMISS;
    } else {
        // invalid current state
        throw runtime_error("invalid state");
    }
}

unsigned int MESIProtocol::getNumShared() { return numShared; }

unsigned int MESIProtocol::getNumPrivate() { return numPrivate; }
