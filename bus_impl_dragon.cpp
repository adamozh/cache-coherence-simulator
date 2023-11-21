
#include "bus_impl_dragon.hpp"

using namespace std;

bool dragon_bus_debug = true;

bool BusImplDragon::checkCacheBlocked(unsigned int indexWithTag){
    // Find the address in the unordered_map
    auto it = cacheBlocked.find(indexWithTag);

    // If address is found, return true, else return false
    return it != cacheBlocked.end();
}

void BusImplDragon::addCacheBlocked(unsigned int indexWithTag, int pid){
    // add the address in the unordered_map processorModifiedCache if does not exist
    cacheBlocked.insert({indexWithTag, pid});
}

void BusImplDragon::removeCacheBlocked(unsigned int indexWithTag, int pid){
    // remove the address in the unordered_map processorModifiedCache if it exist
    auto it = cacheBlocked.find(indexWithTag);
    if (it != cacheBlocked.end()) {
        cacheBlocked.erase(it);
    }
}

void BusImplDragon::executeCycle() {
    if (dragon_bus_debug) cout << "execute bus cycle" << endl;
    if (currReq == nullptr && busQueue.empty() && memRequests.empty()) {
        return;
    }

    // pop if no current request
    if (currReq == nullptr && !busQueue.empty()) {
        currReq = busQueue.front();
        busQueue.pop();
        processRequest(currReq);
    }

    // decrement current request if there is one
    if (currReq != nullptr) {
        currReq->countdown--;
        if (currReq->countdown == 0) {
            if (currReq->isToMemOrCache) {
                // this request is going to mem next (100)
                memRequests[currReq->pid] = currReq;
            } else {
                // this request is going to cache (finished the last 2n, done)
                // sanity check: this is the only place where request is set to done
                currReq->done = true;
                // unlock cache blocked over here.
                if (currReq->type == BusUpd){
                    removeCacheBlocked(currReq->address/wordsPerBlock,currReq->pid);
                } 
            }
            currReq = nullptr;
            return;
        }
    }

    // decrement memory accesses
    for (int i = 0; i < memRequests.size(); i++) {
        auto req = memRequests[i];
        if (req == nullptr) continue;
        req->countdown--;
        if (req->countdown == 0) {
            req->countdown = 2 * wordsPerBlock;
            req->isToMemOrCache = false;
            // this last 2n request is still tagged to the original processor in the bus map
            busQueue.push(req);
            // remove the request from memory, its now in the bus
            memRequests[i] = nullptr;
        }
    }
}

void BusImplDragon::processBusRd(shared_ptr<Request> request) {
    // need to check the line is shared
    if (dragon_bus_debug) cout << "processing BusRd" << endl;
    // check if any other processor has it in M state
    // sanity check : if there exists the block in M state then they need to flush it
    // otherwise, everyone who has the block MUST match memory. so 100 + 2n
    bool isModified = false; // used to decide where the request goes
    bool isShared = false;   // used to decide the new state of the current processor
    //check whether the line is shared
    for (auto p : this->processors) {
        //skip if the processor is the current processor
        if (p->getPID() == request->pid){
            continue;
        }
        State pState = p->getState(request->address);
        isShared |= (pState == M || pState == E || pState == Sm || pState == Sc);
    }
    for (auto p : this->processors) {
        State pState = p->getState(request->address);
        if (isShared){
            if (pState == M){
                p->setState(request->address, Sm);
            }else if(pState == E){
                p->setState(request->address, Sc);
            }
        }    
    }
    State newState = isShared ? Sc : E;
    processors[request->pid]->setState(request->address, newState);
    if (isShared) { //for dragon, if its shared, it will be 2n else it will be 2n + 100
        // this case is 2n
        // this request is spending 2n on the bus, and then going to memory
        request->countdown = 2 * wordsPerBlock;
        request->isToMemOrCache = false;
    } else {
        // load from memory, this case is 100 + 2n
        request->countdown = 100;
        request->isToMemOrCache = false;
        memRequests[request->pid] = request;
        currReq = nullptr;
    }
}

void BusImplDragon::processBusUpd(shared_ptr<Request> request) {
    if (dragon_bus_debug) cout << "processing BusUpd" << endl;
    bool isModified = false; // used to decide where the request goes
    
    // send updated word over, this case is 2
    request->countdown = 2;
    request->isToMemOrCache = true;
    currReq = nullptr; // TODO: what is this used for
}

void BusImplDragon::processRequest(shared_ptr<Request> request) {
    if (!request->isToMemOrCache) return; // this is going back to cache, nothing to do
    if (request->type == BusRd) {
        processBusRd(request);
    } else if (request->type == BusUpd) {
        processBusUpd(request);
    }
}

void BusImplDragon::updateOtherCachesToSc(unsigned int indexWithTag,int pid){
    auto request = currReq;
    for (auto p : this->processors) {
        //skip if the processor is the current processor
        if (p->getPID() == request->pid){
            continue;
        }
        State pState = p->getState(request->address);
        if (pState == Sm){
            p->setState(request->address, Sc);
        }
    }
}

