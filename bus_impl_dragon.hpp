#pragma once

#ifndef BUS_IMPL_DRAGON_H
#define BUS_IMPL_DRAGON_H

#include "bus_impl.hpp"
#include "processor.hpp"
#include "request.hpp"
#include <iostream>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

class BusImplDragon : public BusImpl {
    // Additional functionalities or modifications here
  private: // might want to change it to protected so that the child class can access the variables
    unsigned int associativity;
    unsigned int cachesize;
    unordered_map<unsigned int, int> cacheBlocked; // check whether memory has been modified
    // unordered_map<unsigned int, int> processorModifiedCache;// know which processor has Sm

    

  protected:
    void processBusRd(shared_ptr<Request> request) override;
    void processBusUpd(shared_ptr<Request> request);

  public:
    // void executeCycle() override;
    BusImplDragon(int wordsPerBlock, unsigned int associativity, unsigned int cachesize)
        : BusImpl(wordsPerBlock), associativity(associativity),
          cachesize(cachesize / (associativity * wordsPerBlock)){};
    bool checkCacheBlocked(unsigned int indexWithTag, int pid) override;
    void addCacheBlocked(unsigned int indexWithTag, int pid) override;
    void removeCacheBlocked(unsigned int indexWithTag, int pid) override;
    void updateOtherCachesToSc(unsigned int address, int pid) override;
    void executeCycle() override;
    void processRequest(shared_ptr<Request> request);
};

#endif // BUS_IMPL_DRAGON_H
