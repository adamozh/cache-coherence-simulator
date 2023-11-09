#pragma once

#include "processor.hpp"
#include <memory>
#include <vector>

using namespace std;

class Bus {
  private:
    vector<shared_ptr<Processor>> processors;

  public:
    virtual void attachProcessor(shared_ptr<Processor> proc) = 0;
    virtual void issueInvalidation() = 0;
    virtual bool issueBusRd(unsigned int address) = 0;
    virtual void executeCycle() = 0;
};
