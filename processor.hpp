#pragma once

class Processor {
  public:
    virtual void executeCycle() = 0;
    virtual void invalidateCache() = 0;
    virtual bool isDone() = 0;
};
