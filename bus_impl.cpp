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

void BusImpl::executeCycle() {
    if (currReq == nullptr) {
        currReq = busQueue.front();
        busQueue.pop();
    }
    currReq->decrement();
    if (currReq->isDone()) {
        /* current bus transaction is done */
    }
    /* handle memory */
}
