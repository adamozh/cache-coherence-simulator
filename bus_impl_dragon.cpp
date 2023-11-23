
#include "bus_impl_dragon.hpp"
#include <unistd.h>

using namespace std;

bool dragon_bus_debug = false;

bool BusImplDragon::checkCacheBlocked(unsigned int indexWithTag, int pid) {
    // Find the address in the unordered_map
    auto it = cacheBlocked.find(indexWithTag);

    // If address is found, return true, else return false
    if (it != cacheBlocked.end()) {
        // check if it is by the user ///TODO:
        return cacheBlocked[indexWithTag] != pid;
    }
    return false;
}

void BusImplDragon::addCacheBlocked(unsigned int indexWithTag, int pid) {
    // add the address in the unordered_map processorModifiedCache if does not exist
    cacheBlocked.insert({indexWithTag, pid});
}

void BusImplDragon::removeCacheBlocked(unsigned int indexWithTag, int pid) {
    // remove the address in the unordered_map processorModifiedCache if it exist
    auto it = cacheBlocked.find(indexWithTag);
    if (it != cacheBlocked.end()) {
        cacheBlocked.erase(it);
    }
}

void BusImplDragon::executeCycle() {
    if (dragon_bus_debug)
        cout << "execute bus cycle--"
             << "busqueuesize: " << busQueue.size() << " memqueuesize: " << memRequests.size()
             << " curr request " << (currReq == nullptr) << endl;
    if (dragon_bus_debug) {
        for (const auto &pair : cacheBlocked) {
            std::cout << "{" << pair.first << ": " << pair.second << "} ";
        }
        std::cout << std::endl;
    }
    if (currReq == nullptr && busQueue.empty() && memRequests.empty()) {
        return;
    }

    // pop if no current request
    if (currReq == nullptr && !busQueue.empty()) {
        if (dragon_bus_debug)
            cout << "from execute cycle: -- busqueue is  " << busQueue.size() << endl;
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
            }
            if (currReq->pid == -2) {
                removeCacheBlocked(currReq->address / wordsPerBlock, currReq->pid);
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

    if (request->pid == -1) {
        // countdown of passing through the bus only
        request->countdown = 2 * wordsPerBlock;
        request->isToMemOrCache = false;
        trafficInBytes += 4 * wordsPerBlock;
        return;
    }

    // check if any other processor has it in M state
    // sanity check : if there exists the block in M state then they need to flush it
    // otherwise, everyone who has the block MUST match memory. so 100 + 2n
    bool isModified = false; // used to decide where the request goes
    bool isShared = false;   // used to decide the new state of the current processor
    // check whether the line is shared
    for (auto p : this->processors) {
        // skip if the processor is the current processor
        if (p->getPID() == request->pid) {
            continue;
        }
        State pState = p->getState(request->address);
        isShared |= (pState == M || pState == E || pState == Sm || pState == Sc);
    }

    for (auto p : this->processors) {
        State pState = p->getState(request->address);
        if (isShared) {
            if (pState == M) {
                p->setState(request->address, Sm);
            } else if (pState == E) {
                p->setState(request->address, Sc);
            }
        }
    }

    if (dragon_bus_debug) cout << "processingRd" << request->pid << endl;
    State newState = isShared ? Sc : E;
    if (newState == Sc) {
        numShared++;
    } else if (newState == E) {
        numPrivate++;
    }
    processors[request->pid]->addCacheLine(request->address, newState);

    if (isShared) { // for dragon, if its shared, it will be 2n else it will be 2n + 100
        // this case is 2n
        // this request is spending 2n on the bus, and then going to memory
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
    if (dragon_bus_debug) cout << "processingRd" << request->pid << endl;
}

void BusImplDragon::processBusUpd(shared_ptr<Request> request) {
    if (dragon_bus_debug) cout << "processing BusUpd" << endl;
    if (request->pid == -2) {

        if (dragon_bus_debug) cout << "request does goes through here" << endl;
        request->countdown = 2;
        request->isToMemOrCache = false;
        trafficInBytes += 4;
        return;
    }
    // bool isModified = false; // used to decide where the request goes
    // // check if the line is shared
    // // send updated word over, this case is 2
    // request->countdown = 2;
    // request->isToMemOrCache = true;
    // currReq = nullptr; // TODO: what is this used for
    // check if any other processor has it in M state
    // sanity check : if there exists the block in M state then they need to flush it
    // otherwise, everyone who has the block MUST match memory. so 100 + 2n
    bool isModified = false; // used to decide where the request goes
    bool isShared = false;   // used to decide the new state of the current processor
    // check whether the line is shared
    for (auto p : this->processors) {
        // skip if the processor is the current processor
        if (p->getPID() == request->pid) {
            continue;
        }
        State pState = p->getState(request->address);
        isShared |= (pState == M || pState == E || pState == Sm || pState == Sc);
    }
    // check for request pid
    if (request->pid != -1 && request->pid != -2) { // this will be from the I state
        for (auto p : this->processors) {
            State pState = p->getState(request->address);
            if (isShared) {
                p->setState(request->address, M);
            } else {
                p->setState(request->address, Sm);
                // lock cache over here
            }
        }
        if (dragon_bus_debug) cout << "processingUpd " << request->pid << endl;
        State newState = isShared ? Sm : M;
        if (newState == Sm) {
            numShared++;
        } else if (newState == M) {
            numPrivate++;
        }
        processors[request->pid]->addCacheLine(request->address, newState);
        if (isShared) { // for dragon, if its shared, it will be 2n else it will be 2n + 100
            // this case is 2n
            // this request is spending 2n on the bus, and then going to memory
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
    } else { // sending message only
        request->countdown = 2;
        request->isToMemOrCache = false;
        currReq = nullptr;
    }
}

void BusImplDragon::processRequest(shared_ptr<Request> request) {
    if (dragon_bus_debug) cout << "This is from the dragon child class" << endl;
    if (!request->isToMemOrCache) return; // this is going back to cache, nothing to do
    if (request->type == BusRd) {
        processBusRd(request);
    } else if (request->type == BusUpd) {
        processBusUpd(request);
    }
}

void BusImplDragon::updateOtherCachesToSc(unsigned int address, int pid) {
    for (auto p : this->processors) {
        // skip if the processor is the current processor
        if (p->getPID() == pid) {
            continue;
        }
        State pState = p->getState(address);
        if (pState == Sm) {
            p->setState(address, Sc);
        }
    }
}
