#pragma once
#include "bus.hpp"
#include "cache.hpp"
#include "request.hpp"
#include <memory>

using namespace std;

class Protocol {
  public:
    virtual shared_ptr<Request> onLoad(int pid, unsigned int address, shared_ptr<Bus> bus,
                                       shared_ptr<Cache> cache) = 0;
    virtual shared_ptr<Request> onStore(int pid, unsigned int address, shared_ptr<Bus> bus,
                                        shared_ptr<Cache> cache) = 0;
    virtual bool onBusRd(unsigned int address, shared_ptr<Bus> bus, shared_ptr<Cache> cache) = 0;
};
