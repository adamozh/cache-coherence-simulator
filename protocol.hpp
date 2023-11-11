#pragma once
#include "bus.hpp"
#include "cache.hpp"
#include "request.hpp"
#include <memory>

using namespace std;

class Protocol {
  public:
    virtual bool onLoad(int pid, unsigned int address, shared_ptr<Bus> bus,
                        shared_ptr<Cache> cache) = 0;
    virtual bool onStore(int pid, unsigned int address, shared_ptr<Bus> bus,
                         shared_ptr<Cache> cache) = 0;
};
