#include "mesi.hpp"
#include "state.hpp"
#include <stdexcept>

using namespace std;

void MESIProtocol::onLoad(unsigned int address, shared_ptr<Bus> bus, shared_ptr<Cache> cache) {
    State state = cache->getCacheLineState(address);
    if (state == M || state == E || state == S) {
        return;
    } else if (state == I) {
        bool isExclusive = bus->issueBusRd(address);
        State newState = isExclusive ? E : S;
        cache->setCacheLineState(address, newState);
    } else {
        throw runtime_error("invalid state");
    }
}

void MESIProtocol::onStore(unsigned int address, shared_ptr<Bus> bus, shared_ptr<Cache> cache) {
    // MESI-specific logic
    /*
    if hit ->
        if E -> change to M, return
        else -> return
    else ->
        create BusRdX request

    */
}

bool MESIProtocol::onBusRd(unsigned int address, shared_ptr<Bus>, shared_ptr<Cache> cache) {
    State state = cache->getCacheLineState(address);
    if (state == M || state == E || state == S) {
        return true;
    }
    return false;
}
