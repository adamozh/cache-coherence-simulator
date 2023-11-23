#include "bus_impl_moesi.hpp"
#include "processor.hpp"
#include "request.hpp"
#include "state.hpp"
#include <iostream>
#include <memory>
#include <queue>
#include <stdexcept>
#include <unistd.h>
#include <vector>

bool moesi_bus_debug = false;

void BusImplMOESI::processBusRd(shared_ptr<Request> request) {
    if (moesi_bus_debug) cout << "processing BusRd from moesi" << endl;
    // check if any other processor has it in M state
    // sanity check : if there exists the block in M state then they need to flush it
    // otherwise, everyone who has the block MUST match memory. so 100 + 2n
    bool isShared = false;   // used to decide the new state of the current processor
    bool isOwned = false;
    for (auto p : this->processors) {
        State pState = p->getState(request->address);
        isShared |= (pState == M || pState == E || pState == S || pState == O);
        if (pState == M || pState == O){
            p->setState(request->address, O);
        } else {
            p->setState(request->address, S); 
        }
        isOwned |= (pState == O || pState == M || pState == E);
    }
    State newState = isShared ? S : E;
    if (newState == S) {
        numShared++;
    } else if (newState == E) {
        numPrivate++;
    }
    processors[request->pid]->addCacheLine(request->address, newState);
    if (isOwned) {
        // 2n
        request->countdown = 2 * wordsPerBlock;
        request->isToMemOrCache = false;
        trafficInBytes += 4 * wordsPerBlock;
    } else {
        // load from memory, this case is 100 + 2n
        request->countdown = 100;
        request->isToMemOrCache = false;
        memRequests[request->pid] = request;
        currReq = nullptr;
    }
}

void BusImplMOESI::processBusRdX(shared_ptr<Request> request) { // O and S
    if (moesi_bus_debug) cout << "processing BusRdX" << endl;
    bool isOwned = false;
    for (auto p : this->processors) { // P1 O -> M P2 S
        State pState = p->getState(request->address);
        isOwned |= (pState == M || pState == O);

    }
    for (auto p : this->processors) { // P1 O -> M P2 S
        if (p->getPID() == request->pid){ //like that?
            continue;
        }
        State pState = p->getState(request->address);
        bool isInvalidated = p->invalidateCache(request->address);
        if (isInvalidated) {
            numInvalidationsOrUpdates++;
        }
    }
    // processors[request->pid]->setState(request->address, M);
    processors[request->pid]->addCacheLine(request->address, M);
    numPrivate++;
    if (isOwned) {
        // cache-to-cache, this case is 2n
        // this request is spending 2n on the bus, and then going to memory
        request->countdown = 2 * wordsPerBlock;
        request->isToMemOrCache = true;
        trafficInBytes += 4 * wordsPerBlock;
    } else {
        // load from memory, this case is 100 + 2n
        request->countdown = 100;
        request->isToMemOrCache = false;
        memRequests[request->pid] = request;
        currReq = nullptr;
    }
}
