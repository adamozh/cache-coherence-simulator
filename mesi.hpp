#include "cache.hpp"
#include "protocol.hpp"
#include <memory>

class MESIProtocol : public Protocol {
  public:
    void onLoad(unsigned int address, shared_ptr<Bus> bus, shared_ptr<Cache> cache) override;
    void onStore(unsigned int address, shared_ptr<Bus> bus, shared_ptr<Cache> cache) override;
};