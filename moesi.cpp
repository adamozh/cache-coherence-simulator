#include "moesi.hpp"
#include "request.hpp"
#include "state.hpp"
#include <stdexcept>


using namespace std;

bool moesi_debug = true;

CacheResultType MOESIProtocol::onLoad(int pid, unsigned int address, shared_ptr<Bus> bus,
                                     shared_ptr<Cache> cache) {
    State state = cache->getCacheLineState(address);
    if (state == M || state == E || state == S|| state == O) {
        if (moesi_debug) cout << "M/E/S: load hit" << endl;
        return CACHEHIT;
    } else if (state == I) {
        if (moesi_debug) cout << "I: load miss, pushing BusRd" << endl;
        shared_ptr<Request> busRdRequest = make_shared<Request>(pid, BusRd, address);
        bus->pushRequestToBus(busRdRequest);
        return CACHEMISS;
    } else {
        throw runtime_error("invalid state");
    }
}

CacheResultType MOESIProtocol::onStore(int pid, unsigned int address, shared_ptr<Bus> bus,
                                      shared_ptr<Cache> cache) {
    State state = cache->getCacheLineState(address);
    if (state == M) {
        if (moesi_debug) cout << "M: store hit" << endl;
        cache->updateCacheLine(address,M);
        bus->issueInvalidation(pid,address);
        return CACHEHIT;
    } else if (state == E) {
        if (moesi_debug) cout << "E: store hit, change to M" << endl;
        cache->updateCacheLine(address,M);
        bus->issueInvalidation(pid,address);
        return CACHEHIT;
    } else if (state == O){
        if (moesi_debug) cout << "E: store hit, change to M" << endl;
        cache->updateCacheLine(address,M);
        bus->issueInvalidation(pid,address);
        return CACHEHIT;
    } else if (state == S) {
        if (moesi_debug) cout << "S: store miss, pushing BusRdX" << endl;
        // BusRdX is only processed when popped from bus
        shared_ptr<Request> busRdXRequest = make_shared<Request>(pid, BusRdX, address);
        bus->pushRequestToBus(busRdXRequest);
        return CACHEMISS;
    } else if (state == I) {
        if (moesi_debug) cout << "I: store miss, pushing BusRdX" << endl;
        shared_ptr<Request> busRdXRequest = make_shared<Request>(pid, BusRdX, address);
        bus->pushRequestToBus(busRdXRequest);
        return CACHEMISS;
    } else {
        // invalid current state
        throw runtime_error("invalid state");
    }
}
