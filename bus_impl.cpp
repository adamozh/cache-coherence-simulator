#include "bus_impl.hpp"
#include "processor.hpp"
#include "request.hpp"
#include "state.hpp"
#include <iostream>
#include <memory>
#include <queue>
#include <stdexcept>
#include <vector>

using namespace std;

bool bus_debug = true;

/**
 * correctness checks for implementation of bus:
 * 1. handling of requests in bus queue, current request, memory requests
 * 2. lifecycle of a request, from creation to being set to done (and countdown values)
 */

void BusImpl::attachProcessor(shared_ptr<Processor> proc) {
    this->processors.push_back(proc);
    this->currentRequests.push_back(nullptr);
    this->memRequests.push_back(nullptr);
}

void BusImpl::pushRequestToBus(shared_ptr<Request> request) {
    busQueue.push(request);
    // the request is tagged to the processor for tracking. it should remain the same throughout
    if (request->pid != -1 && request->pid != -2) {
        currentRequests[request->pid] = request;
    }
}

bool BusImpl::isCurrentRequestDone(int pid) {
    if (pid == -1) {
        throw runtime_error("pid shouldn't be -1");
    }
    if (currentRequests[pid] == nullptr) {
        throw runtime_error("there should be a current request");
    }
    return currentRequests[pid]->done;
}

void BusImpl::issueInvalidation(unsigned int pid, unsigned int address) {
    if (bus_debug) cout << "invalidating cache! " << endl;
    for (auto processor : this->processors) {
        if (processor->getPID() == pid) {
            continue;
        }
        processor->invalidateCache(address); // Not sure if this will send through the bus
    }
}

void BusImpl::processBusRd(shared_ptr<Request> request) {
    if (bus_debug) cout << "processing BusRd" << endl;
    // check if any other processor has it in M state
    // sanity check : if there exists the block in M state then they need to flush it
    // otherwise, everyone who has the block MUST match memory. so 100 + 2n
    bool isModified = false; // used to decide where the request goes
    bool isShared = false;   // used to decide the new state of the current processor
    for (auto p : this->processors) {
        State pState = p->getState(request->address);
        isModified |= (pState == M);
        isShared |= (pState == M || pState == E || pState == S);
        p->setState(request->address, S); // does nothing if block not in cache
    }
    State newState = isShared ? S : E;
    if (newState == S) {
        numShared++;
    } else if (newState == E) {
        numPrivate++;
    }
    processors[request->pid]->addCacheLine(request->address, newState);
    if (isModified) {
        // flush, this case is 2n + 100 + 2n
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

void BusImpl::processBusRdX(shared_ptr<Request> request) {
    if (bus_debug) cout << "processing BusRdX" << endl;
    bool isModified = false; // used to decide where the request goes
    for (auto p : this->processors) {
        State pState = p->getState(request->address);
        isModified |= (pState == M);
        // if M, no flush happens here, flush is simulated below
        bool isInvalidated = p->invalidateCache(request->address);
        if (isInvalidated) {
            numInvalidationsOrUpdates++;
        }
    }
    numPrivate++;
    processors[request->pid]->addCacheLine(request->address, M);
    if (isModified) {
        // flush, this case is 2n + 100 + 2n
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

void BusImpl::processRequest(shared_ptr<Request> request) {
    // if processor is -1 then just pass it through the bus and disappear
    if (request->pid == -1) {
        // countdown of passing through the bus only
        request->countdown = 2 * wordsPerBlock;
        request->isToMemOrCache = false;
        trafficInBytes += 4 * wordsPerBlock;
        return;
    }
    if (!request->isToMemOrCache) return; // this is going back to cache, nothing to do
    if (request->type == BusRd) {
        processBusRd(request);
    } else if (request->type == BusRdX) {
        processBusRdX(request);
    }
}

void BusImpl::executeCycle() {
    if (bus_debug)
        cout << "bus: "
             << "busQueue.size is " << busQueue.size() << " Memqueue.size " << memRequests.size()
             << " curr_request " << currReq << " " << nullptr << " currreq equal to nullptr? "
             << (currReq == nullptr) << endl;
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
                currReq->countdown = 100;
                currReq->isToMemOrCache = false;
            } else {
                // this request is going to cache (finished the last 2n, done)
                // sanity check: this is the only place where request is set to done
                currReq->done = true;
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

void BusImpl::printProgress() {
    int pid = currReq == nullptr ? -1 : currReq->pid;
    cout << "Current bus request is from processor: " << pid << endl;
    cout << "All current requests in bus/memory: ";
    for (auto r : currentRequests) {
        if (r == nullptr) continue;
        string done = r->done ? "done" : "not done";
        cout << "[" << r->pid << ": " << r->countdown << " " << done << "] ";
    }
    cout << endl;
    cout << "Queue size of bus: " << busQueue.size() << endl;
    cout << "Requests in memory: ";
    for (auto r : memRequests) {
        if (r == nullptr) continue;
        cout << r->pid << ":" << r->countdown << " ";
    }
    cout << endl;
}

shared_ptr<Processor> BusImpl::getProcessor(int pid) { return processors[pid]; }

void BusImpl::printStatistics() {
    cout << "BUS" << endl;
    cout << "Traffic on the bus (in bytes): " << trafficInBytes << endl;
    cout << "Number of invalidations/updates: " << numInvalidationsOrUpdates << endl;
}

unsigned int BusImpl::getNumShared() { return numShared; }

unsigned int BusImpl::getNumPrivate() { return numPrivate; }
