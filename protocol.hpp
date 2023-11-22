#pragma once
#include "bus.hpp"
#include "cache.hpp"
#include "cache_result_type.hpp"
#include "request.hpp"
#include <memory>

using namespace std;

class Protocol {
  public:
    virtual CacheResultType onLoad(int pid, unsigned int address, shared_ptr<Bus> bus,
                                   shared_ptr<Cache> cache) = 0;
    virtual CacheResultType onStore(int pid, unsigned int address, shared_ptr<Bus> bus,
                                    shared_ptr<Cache> cache) = 0;
    virtual unsigned int getNumShared() = 0;
    virtual unsigned int getNumPrivate() = 0;
};
