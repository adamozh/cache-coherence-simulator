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
    void processBusRd(shared_ptr<Request> request);
    void processBusRdX(shared_ptr<Request> request);

  public:
    int wordsPerBlock;
    vector<shared_ptr<Processor>> processors;
    vector<shared_ptr<Request>> currentRequests;
    shared_ptr<Request> currReq = nullptr;
    vector<shared_ptr<Request>> memRequests;
    queue<shared_ptr<Request>> busQueue;
    void processRequest(shared_ptr<Request> request);

  public:
    BusImpl(int wordsPerBlock) : wordsPerBlock(wordsPerBlock){};
    void attachProcessor(shared_ptr<Processor> proc) override;
    void issueInvalidation(unsigned int pid, unsigned int address) override;
    void pushRequestToBus(shared_ptr<Request> request) override;
    bool isCurrentRequestDone(int pid) override;
    void executeCycle() override;
    virtual shared_ptr<Processor> getProcessor(int pid);
    void printProgress() override;
    bool checkCacheBlocked(unsigned int indexWithTag) override { return true; };
    void addCacheBlocked(unsigned int indexWithTag, int pid) override{};
    void removeCacheBlocked(unsigned int indexWithTag, int pid) override{};
    void updateOtherCachesToSc(unsigned int indexWithTag, int pid) override{};
};
