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
    if (request->pid != -1) {
        currentRequests[request->pid] = request;
    }
}

// sanity check: at least for mesi, this is not used becausae protocol/processor won't call.
// because they haven't done any checks for whether another processor has it in M state
// this is done internally in this class, in processBusRd and processBusRdX
// i'll leave it here first but if you don't need this, can remove
void BusImpl::pushRequestToMemory(shared_ptr<Request> request) {
    memRequests.push_back(request);
    if (request->pid != -1) {
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

void BusImpl::issueInvalidation(unsigned int pid) {
    for (auto processor : this->processors) {
        if (processor->getPID() == pid) {
            continue;
        }
        processor->invalidateCache();
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
        p->setState(request->address, S);
    }
    State newState = isShared ? S : E;
    processors[request->pid]->setState(request->address, newState);
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

void BusImpl::processBusRdX(shared_ptr<Request> request) {
    if (bus_debug) cout << "processing BusRdX" << endl;
    bool isModified = false; // used to decide where the request goes
    for (auto p : this->processors) {
        State pState = p->getState(request->address);
        isModified |= (pState == M);
        p->setState(request->address, I); // invalidation happens here
    }
    processors[request->pid]->setState(request->address, M);
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

void BusImpl::processRequest(shared_ptr<Request> request) {
    if (!request->isToMemOrCache) return; // this is going back to cache, nothing to do
    if (request->type == BusRd) {
        processBusRd(request);
    } else if (request->type == BusRdX) {
        processBusRdX(request);
    }
}

void BusImpl::executeCycle() {
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
    cout << "Queue size: " << busQueue.size() << endl;
    cout << "Requests in memory: ";
    for (auto r : memRequests) {
        if (r == nullptr) continue;
        cout << r->pid << ":" << r->countdown << " ";
    }
    cout << endl;
}

shared_ptr<Processor> BusImpl::getProcessor(int pid) { return processors[pid]; }
