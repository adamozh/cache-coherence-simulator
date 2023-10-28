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
    vector<Request> memRequests;
    queue<shared_ptr<Request>> busQueue;

  public:
    void attachProcessor(shared_ptr<Processor> proc);
    void issueInvalidation();
    void executeCycle();
};
