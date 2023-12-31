#pragma once
#include "cache_result_type.hpp"
#include "state.hpp"

class Processor {
  public:
    virtual void executeCycle() = 0;
    virtual bool invalidateCache(unsigned int address) = 0;
    virtual State getState(unsigned int address) = 0;
    virtual bool isDone() = 0;
    virtual void setState(unsigned int address, State state) = 0;
    virtual void addCacheLine(unsigned int address, State state) = 0;
    virtual int getPID() = 0;
    virtual void printProgressInline() = 0;
    virtual void printStatistics() = 0;
    virtual unsigned int getNumComputeCycles() = 0;
    virtual unsigned int getNumIdle() = 0;
    virtual unsigned int getNumLoad() = 0;
    virtual unsigned int getNumStore() = 0;
};
