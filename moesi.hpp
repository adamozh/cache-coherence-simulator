#pragma once
#include "cache.hpp"
#include "protocol.hpp"
#include <memory>
#include "cache_result_type.hpp"
using namespace std;

class MOESIProtocol : public Protocol {
  public:
    CacheResultType onLoad(int pid, unsigned int address, shared_ptr<Bus> bus,
                shared_ptr<Cache> cache) override;
    CacheResultType onStore(int pid, unsigned int address, shared_ptr<Bus> bus,
                 shared_ptr<Cache> cache) override;
};
