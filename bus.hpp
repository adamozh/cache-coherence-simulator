#pragma once

#include "processor.hpp"
#include "request.hpp"
#include <memory>
#include <vector>

using namespace std;

class Bus {
  private:
    vector<shared_ptr<Processor>> processors;

  public:
    virtual void attachProcessor(shared_ptr<Processor> proc) = 0;
    virtual void pushRequestToBus(shared_ptr<Request> request) = 0;
    virtual void pushRequestToMemory(shared_ptr<Request> request) = 0;
    virtual void issueInvalidation(unsigned int address) = 0;
    virtual void executeCycle() = 0;
    virtual bool isCurrentRequestDone(int pid) = 0;
    virtual void printProgress() = 0;
};
