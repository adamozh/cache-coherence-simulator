#pragma once

class Processor {
  private:
    int id;

  public:
    virtual void executeCycle() = 0;
    virtual void invalidateCache() = 0;
    virtual bool isDone() = 0;
};
