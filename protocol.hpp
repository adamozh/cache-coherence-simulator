#pragma once
#include "bus.hpp"
#include "cache.hpp"
#include "request.hpp"
#include <memory>
#include "cache_result_type.hpp"
using namespace std;

class Protocol {
  public:
    virtual CacheResultType onLoad(int pid, unsigned int address, shared_ptr<Bus> bus,
                        shared_ptr<Cache> cache) = 0;
    virtual CacheResultType onStore(int pid, unsigned int address, shared_ptr<Bus> bus,
                         shared_ptr<Cache> cache) = 0;
};
