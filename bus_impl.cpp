#include "bus_impl.hpp"
#include "processor.hpp"
#include "request.hpp"
#include "state.hpp"
#include <memory>
#include <queue>
#include <stdexcept>
#include <vector>

using namespace std;

void BusImpl::attachProcessor(shared_ptr<Processor> proc) { this->processors.push_back(proc); }

void BusImpl::pushRequestToBus(shared_ptr<Request> request) {
    busQueue.push(request);
    if (request->pid != -1) {
        currentRequests[request->pid] = request;
    }
}

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

/*
 * assume that processing a bus request happens within one cycle
 */
void BusImpl::processBusRd(shared_ptr<Request> request) {
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
        memRequests.push_back(request);
        currReq = nullptr;
    }
}

void BusImpl::processBusRdX(shared_ptr<Request> request) { return; }

void BusImpl::processRequest(shared_ptr<Request> request) {
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

    // handle current request if there is one
    if (currReq != nullptr) {
        currReq->countdown--;
        if (currReq->countdown == 0) {
            if (currReq->isToMemOrCache) {
                memRequests.push_back(currReq);
            } else {
                currReq->done = true;
            }
            currReq = nullptr;
            return;
        }
    }

    // decrement memory accesses
    for (auto req : memRequests) {
        req->countdown--;
        if (req->countdown == 0) {
            req->countdown = 2 * wordsPerBlock;
            req->isToMemOrCache = false;
            // this last 2n request is still tagged to the original processor in the bus map
            busQueue.push(req);
        }
    }
}
