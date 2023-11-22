
#include "dragon.hpp"

using namespace std;

bool dragon_debug = true;

CacheResultType DragonProtocol::onLoad(int pid, unsigned int address, shared_ptr<Bus> bus,
                                       shared_ptr<Cache> cache) {
    State state = cache->getCacheLineState(address);
    // handle counters
    if (state == M || state == E) {
        numPrivate++;
    } else if (state == Sm|| state == Sc) {
        numShared++;
    }

    // actual processing
    if (state == M || state == E || state == Sm) {
        if (dragon_debug) cout << "M/E/Sm: load hit" << endl;
        cache->updateCacheLine(address, state);
        return CACHEHIT;
    } else if (state == Sc) {
        // check if it is store in the dictionary, if is stored it will be stalled.
        size_t indexWithTag = cache->getIndexWithTag(address);
        bool isCacheBlocked = bus->checkCacheBlocked(indexWithTag,pid);
        cout << "cache is blocked for processor " << pid << "isblocked: " << isCacheBlocked << endl;
        if (!isCacheBlocked) {
            if (dragon_debug) cout << "Sc: load hit" << endl;
            cache->updateCacheLine(address, state);
            return CACHEHIT;
        }
        if (dragon_debug) cout << "Sc: load blocked" << endl;
        return CACHEBLOCKED;

    } else if (state == I) {
        if (dragon_debug) cout << "I: load miss" << endl;
        shared_ptr<Request> busRdRequest = make_shared<Request>(pid, BusRd, address);
        bus->pushRequestToBus(busRdRequest);
        return CACHEMISS;
    } else {
        throw runtime_error("invalid state");
    }
}

CacheResultType DragonProtocol::onStore(int pid, unsigned int address, shared_ptr<Bus> bus,
                                        shared_ptr<Cache> cache) {
    State state = cache->getCacheLineState(address);
    if (state == M) {
        if (dragon_debug) cout << "M: store hit" << endl;
        numPrivate++;
        cache->updateCacheLine(address, state);
        return CACHEHIT;
    } else if (state == Sm){
        if (dragon_debug) cout << "Sm: store hit" << endl;
        size_t indexWithTag = cache->getIndexWithTag(address);
        bus->addCacheBlocked(indexWithTag, pid); // block the cache from other modifications
        shared_ptr<Request> busRdRequest = make_shared<Request>(-2, BusUpd, address);
        cache->updateCacheLine(address, state);
        bus->pushRequestToBus(busRdRequest);
        numShared++;
        return CACHEHIT;
    } else if (state == E) {
        // change state to M
        if (dragon_debug) cout << "E: store hit" << endl;
        cache->updateCacheLine(address, M);
        numPrivate++;
        return CACHEHIT;
    } else if (state == Sc) {
        // check if it is store in the dictionary, if is stored it will be stalled.
        size_t indexWithTag = cache->getIndexWithTag(address);
        bool isCacheBlocked = bus->checkCacheBlocked(indexWithTag,pid);
        if (!isCacheBlocked) {
            if (dragon_debug) cout << "Sc: store hit" << endl;
            cache->updateCacheLine(address, Sm);
            // TODO: use bus command to change the state of other caches
            bus->updateOtherCachesToSc(address, pid);
            bus->addCacheBlocked(indexWithTag, pid); // block the cache from other modifications
            shared_ptr<Request> busRdRequest = make_shared<Request>(-2, BusUpd, address);
            bus->pushRequestToBus(busRdRequest);
            numShared++;
            return CACHEHIT;
        }
        return CACHEBLOCKED;
    } else if (state == I) {
        // will also need to check whether other processors have the cache
        if (dragon_debug) cout << "I: store miss" << endl;
        // TODO: check this
        shared_ptr<Request> busRdRequest = make_shared<Request>(pid, BusUpd, address);
        bus->pushRequestToBus(busRdRequest);
        return CACHEMISS;
    } else {
        throw runtime_error("invalid state");
    }
}

unsigned int DragonProtocol::getNumShared() { return numShared; }

unsigned int DragonProtocol::getNumPrivate() { return numPrivate; }
