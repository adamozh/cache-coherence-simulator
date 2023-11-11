#include "cache.hpp"
#include "protocol.hpp"
#include <memory>

using namespace std;

class MESIProtocol : public Protocol {
  public:
    bool onLoad(int pid, unsigned int address, shared_ptr<Bus> bus,
                shared_ptr<Cache> cache) override;
    bool onStore(int pid, unsigned int address, shared_ptr<Bus> bus,
                 shared_ptr<Cache> cache) override;
};
