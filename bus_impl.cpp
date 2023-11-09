#include "bus_impl.hpp"
#include "processor.hpp"
#include "request.hpp"
#include <memory>
#include <queue>
#include <vector>

using namespace std;

void BusImpl::attachProcessor(shared_ptr<Processor> proc) { this->processors.push_back(proc); }

void BusImpl::issueInvalidation() {
    for (auto processor : this->processors) {
        processor->invalidateCache();
    }
}

bool BusImpl::issueBusRd(unsigned int address) {
    bool isShared = false;
    for (auto p : this->processors) {
        isShared |= p->onBusRd(address);
    }
    return false;
}

void BusImpl::processCurrentRequest() {
    // TODO: need to handle the logic for processing a request
    currReq->decrement();
    if (currReq->isDone()) {
        currReq = nullptr;
    }
    /* handle memory */
}

void BusImpl::executeCycle() {
    if (currReq == nullptr && busQueue.empty()) {
        return;
    }
    if (currReq == nullptr && !busQueue.empty()) {
        currReq = busQueue.front();
        busQueue.pop();
    }
    processCurrentRequest();
}