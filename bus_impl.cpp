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
        if (processor->getPID() == pid){
            continue;
        }
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

        // cache->setCacheLineState(address, newState);
    }
}

void BusImpl::executeCycle() {
    if (currReq == nullptr && busQueue.empty()) {
        return;
    }
    if (currReq == nullptr && !busQueue.empty()) {
        currReq = busQueue.front();
        busQueue.pop();
        processRequest(currReq);
    }
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