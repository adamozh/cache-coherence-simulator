#include "mesi.hpp"
#include "request.hpp"
#include "state.hpp"
#include <stdexcept>

using namespace std;

bool mesi_debug = true;

CacheResultType MESIProtocol::onLoad(int pid, unsigned int address, shared_ptr<Bus> bus,
                          shared_ptr<Cache> cache) {
    State state = cache->getCacheLineState(address);
    if (state == M || state == E || state == S) {
        if (mesi_debug) cout << "M/E/S: load hit" << endl;
        return CACHEHIT;
    } else if (state == I) {
        if (mesi_debug) cout << "I: load miss" << endl;
        shared_ptr<Request> busRdRequest = make_shared<Request>(pid, BusRd, address);
        bus->pushRequestToBus(busRdRequest);
        return CACHEMISS;
    } else {
        throw runtime_error("invalid state");
    }
}

CacheResultType MESIProtocol::onStore(int pid, unsigned int address, shared_ptr<Bus> bus,
                           shared_ptr<Cache> cache) {
    if (mesi_debug) cout << "on store" << endl;
    State state = cache->getCacheLineState(address);
    if (state == M) {
        // cache hit
        if (mesi_debug) cout << "M: store hit" << endl;
        return CACHEHIT;
    } else if (state == E) {
        // change cachestate to M and cache hit
        if (mesi_debug) cout << "E: store hit, change to M" << endl;
        return CACHEHIT;
    } else if (state == S) {
        // cache cahchestate to M and invalidate other caches
        bus->issueInvalidation(pid);
        return CACHEHIT;
    } else if (state == I) {
        // cachemiss, need to get from others //TODO: need to remove the current cache too
        // check if others have the same cacheline too
        shared_ptr<Request> busRdRequest = make_shared<Request>(pid, BusRd, address);
        bus->pushRequestToBus(busRdRequest);
    }
    return CACHEMISS;
}
