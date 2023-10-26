#pragma once

#include "processor.hpp"
#include "request.hpp"
#include <memory>
#include <queue>

using namespace std;

class Bus {
  private:
    vector<shared_ptr<Processor>> processors;
    shared_ptr<Request> currReq;
    vector<Request> memRequests;
    queue<Request> busQueue;

  public:
    virtual void attachProcessor(shared_ptr<Processor> proc) = 0;
    virtual void issueInvalidation() = 0;
    virtual void executeCycle() = 0;
    virtual ~Bus(){};
};
