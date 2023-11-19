#pragma once
#include "state.hpp"
class Processor {
  public:
    virtual void executeCycle() = 0;
    virtual void invalidateCache() = 0;
    virtual State getState(unsigned int address) = 0;
    virtual bool isDone() = 0;
    virtual void setState(unsigned int address, State state) = 0;
    virtual int getPID() = 0;
    virtual void printProgressInline() = 0;
};
