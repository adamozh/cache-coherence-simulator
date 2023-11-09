#pragma once

#include "bus.hpp"
#include "processor.hpp"
#include "request.hpp"
#include <memory>
#include <queue>

using namespace std;

class BusImpl : public Bus {
  private:
    vector<shared_ptr<Processor>> processors;
    shared_ptr<Request> currReq = nullptr;
    vector<shared_ptr<Request>> memRequests;
    queue<shared_ptr<Request>> busQueue;
    void processCurrentRequest();

  public:
    void attachProcessor(shared_ptr<Processor> proc);
    void issueInvalidation();
    bool issueBusRd(unsigned int address);
    void executeCycle();
};
