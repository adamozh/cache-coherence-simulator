#pragma once

#include "bus.hpp"
#include "processor.hpp"
#include "request.hpp"
#include <memory>
#include <queue>
#include <unordered_map>

using namespace std;

class BusImpl : public Bus {
  private:
    vector<shared_ptr<Processor>> processors;
    vector<shared_ptr<Request>> currentRequests;
    shared_ptr<Request> currReq = nullptr;
    vector<shared_ptr<Request>> memRequests;
    queue<shared_ptr<Request>> busQueue;
    void processRequest(shared_ptr<Request> request);

  public:
    void attachProcessor(shared_ptr<Processor> proc);
    void issueInvalidation();
    void pushRequestToBus(shared_ptr<Request> request);
    void pushRequestToMemory(shared_ptr<Request> request);
    bool isCurrentRequestDone(int pid);
    void executeCycle();
};
