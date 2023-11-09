#include "mesi.hpp"
#include "request.hpp"
#include "state.hpp"
#include <stdexcept>

using namespace std;

shared_ptr<Request> MESIProtocol::onLoad(int pid, unsigned int address, shared_ptr<Bus> bus,
                                         shared_ptr<Cache> cache) {
    State state = cache->getCacheLineState(address);
    if (state == M || state == E || state == S) {
        return nullptr;
    } else if (state == I) {
        shared_ptr<Request> busRdRequest = make_shared<Request>(pid, BusRd);
        bus->pushRequest(busRdRequest);
        return busRdRequest;
    } else {
        throw runtime_error("invalid state");
    }
}

shared_ptr<Request> MESIProtocol::onStore(int pid, unsigned int address, shared_ptr<Bus> bus,
                                          shared_ptr<Cache> cache) {
    // MESI-specific logic
    /*
    if hit ->
        if E -> change to M, return
        else -> return
    else ->
        create BusRdX request

    */
    return nullptr;
}
