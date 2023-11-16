#include "mesi.hpp"
#include "request.hpp"
#include "state.hpp"
#include <stdexcept>

using namespace std;

bool MESIProtocol::onLoad(int pid, unsigned int address, shared_ptr<Bus> bus,
                          shared_ptr<Cache> cache) {
    State state = cache->getCacheLineState(address);
    if (state == M || state == E || state == S) {
        return true;
    } else if (state == I) {
        shared_ptr<Request> busRdRequest = make_shared<Request>(pid, BusRd,address);
        bus->pushRequest(busRdRequest);
        return false;
    } else {
        throw runtime_error("invalid state");
    }
}

bool MESIProtocol::onStore(int pid, unsigned int address, shared_ptr<Bus> bus,
                           shared_ptr<Cache> cache) {
    // MESI-specific logic
    /*
    if hit ->
        if E -> change to M, return
        else -> return
    else ->
        create BusRdX request

    */
    State state = cache->getCacheLineState(address);
    if (state == M){
        //cache hit
        return true;
    }else if (state == E){
        //change cachestate to M and cache hit
        cache->updateCacheLine(address,M);
        return true;
    }else if (state == S){
        // cache cahchestate to M and invalidate other caches
        bus->issueInvalidation(pid);
        cache->updateCacheLine(address,M);
        return true;
    }else if (state == I){
        //cachemiss, need to get from others
        //check if others have the same cacheline too
        shared_ptr<Request> busRdRequest = make_shared<Request>(pid, BusRd,address);
        bus->pushRequest(busRdRequest);
        //change the state to M
        cache->updateCacheLine(address,M);
    }
    return true;
}
