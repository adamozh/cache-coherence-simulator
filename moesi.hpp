#pragma once
#include "cache.hpp"
#include "cache_result_type.hpp"
#include "protocol.hpp"
#include <memory>
using namespace std;

class MOESIProtocol : public Protocol {
  public:
    unsigned int numShared = 0;
    unsigned int numPrivate = 0;

    CacheResultType onLoad(int pid, unsigned int address, shared_ptr<Bus> bus,
                           shared_ptr<Cache> cache) override;
    CacheResultType onStore(int pid, unsigned int address, shared_ptr<Bus> bus,
                            shared_ptr<Cache> cache) override;
    unsigned int getNumShared() override;
    unsigned int getNumPrivate() override;
};
