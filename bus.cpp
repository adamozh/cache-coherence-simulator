#include "bus.hpp"
#include "processor.hpp"
#include "request.cpp"
#include <memory>
#include <queue>
#include <vector>

using namespace std;

void Bus::attachProcessor(shared_ptr<Processor> proc) { this->processors.push_back(proc); }

void Bus::issueInvalidation() {
    for (auto processor : this->processors) {
        // TODO: invalidate cache line for all processors
    }
}

void Bus::executeCycle() {
    /* handles bus requests */
    /*
       if (curr == null && busqueue.empty) {
       return;
       } else if (curr == null) {
       curr = busqueue.top();
       curr.decrement()
       } else {
       if (curr.done()) {
       curr = nullptr;
       memRequests.push_back(curr);
       } else {
       curr.decrement();
       }
       }
       */
    /* handle memory */
}
