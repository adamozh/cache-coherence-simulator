#include "bus_impl_moesi.hpp"
#include "processor.hpp"
#include "request.hpp"
#include "state.hpp"
#include <iostream>
#include <memory>
#include <queue>
#include <stdexcept>
#include <vector>
#include <unistd.h>
bool moesi_bus_debug = true;
void BusImplMOESI::processBusRd(shared_ptr<Request> request) {
    if (moesi_bus_debug) cout << "processing BusRd from moesi" << endl;
    usleep(5000000);
    // if processor is -1 then just pass it through the bus and disappear
    if (request-> pid == -1){
        // countdown of passing through the bus only
        request->countdown = 2 * wordsPerBlock;
        request->isToMemOrCache = false;
        return;
    }
    // check if any other processor has it in M state
    // sanity check : if there exists the block in M state then they need to flush it
    // otherwise, everyone who has the block MUST match memory. so 100 + 2n
    bool isModified = false; // used to decide where the request goes
    bool isShared = false;   // used to decide the new state of the current processor
    bool isOwned = false;
    for (auto p : this->processors) {
        State pState = p->getState(request->address);
        isModified |= (pState == M);
        isShared |= (pState == M || pState == E || pState == S || pState == O);
        p->setState(request->address, S);
        isOwned |= (pState == O);
    }
    State newState = isShared ? S : E;
    //processors[request->pid]->setState(request->address, newState);
    processors[request->pid]->addCacheLine(request->address, newState);
    if (isModified) {
        // flush, this case is 2n + 100 + 2n
        // this request is spending 2n on the bus, and then going to memory
        request->countdown = 2 * wordsPerBlock;
        request->isToMemOrCache = true;
    } else {
        if (isOwned){
            request->countdown = 2 * wordsPerBlock;
            request->isToMemOrCache = false;
        }else {
        // load from memory, this case is 100 + 2n
            request->countdown = 100;
            request->isToMemOrCache = false;
            memRequests[request->pid] = request;
            currReq = nullptr;
        }
    }
}

void BusImplMOESI::processBusRdX(shared_ptr<Request> request) {
    if (moesi_bus_debug) cout << "processing BusRdX" << endl;
    bool isModified = false; // used to decide where the request goes
    for (auto p : this->processors) {
        State pState = p->getState(request->address);
        isModified |= (pState == M);
        //p->setState(request->address, I); // invalidation happens here
        p->invalidateCache(request->address);
    }
    //processors[request->pid]->setState(request->address, M);
    processors[request->pid]->addCacheLine(request->address, M);
    if (isModified) {
        // flush, this case is 2n + 100 + 2n
        // this request is spending 2n on the bus, and then going to memory
        request->countdown = 2 * wordsPerBlock;
        request->isToMemOrCache = true;
    } else {
        // load from memory, this case is 100 + 2n
        request->countdown = 100;
        request->isToMemOrCache = false;
        memRequests[request->pid] = request;
        currReq = nullptr;
    }
}