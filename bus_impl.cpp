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

void BusImpl::issueInvalidation() {
    for (auto processor : this->processors) {
        processor->invalidateCache();
    }
}

void BusImpl::processRequest(shared_ptr<Request> request) {
    if (request->type == BusRd) {
        bool isShared = false;
        for (auto p : this->processors) {
            isShared |= p->onBusRd(request->address);
            p->setState(request->address, S);
        }
        State newState = isShared ? S : E;
        processors[request->pid]->setState(request->address, newState);
        if (isShared) {
            // flush, this case is 2n + 100 + 2n
            request->countdown = 2; // TODO: GET BLOCK SIZE HERE
        } else {
            // don't need to flush, just load from memory, this case is 100 + 2n
            request->countdown = 100;
            memRequests.push_back(request);
            currReq = nullptr;
        }
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
            if (currReq->isModified) {
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
            req->countdown = 2;
            // this last 2n request is still tagged to the original processor in the bus map
            busQueue.push(req);
        }
    }
}
