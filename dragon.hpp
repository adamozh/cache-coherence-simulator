#pragma once
#include "cache.hpp"
#include "cache_result_type.hpp"
#include "protocol.hpp"
#include <memory>
#include <unordered_map>

using namespace std;

class DragonProtocol : public Protocol {

  public:
    unsigned int numShared = 0;
    unsigned int numPrivate = 0;
    // shared_modified stores all the processors that has shared_modified
    unordered_map<unsigned int, unsigned int> shared_modified;
    // to see if there is shared cacheline and can change to E or M as needed if not shared
    unordered_map<unsigned int, unsigned int> shared_cacheline;

  public:
    CacheResultType onLoad(int pid, unsigned int address, shared_ptr<Bus> bus,
                           shared_ptr<Cache> cache) override;
    CacheResultType onStore(int pid, unsigned int address, shared_ptr<Bus> bus,
                            shared_ptr<Cache> cache) override;
    unsigned int getNumShared() override;
    unsigned int getNumPrivate() override;
};
