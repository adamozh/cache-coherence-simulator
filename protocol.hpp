#pragma once

#include "bus.hpp"
#include "cache.hpp"
#include <memory>

class Protocol {
  public:
    virtual void onLoad(unsigned int address, shared_ptr<Bus> bus, shared_ptr<Cache> cache) = 0;
    virtual void onStore(unsigned int address, shared_ptr<Bus> bus, shared_ptr<Cache> cache) = 0;
    virtual ~Protocol() {}
};
