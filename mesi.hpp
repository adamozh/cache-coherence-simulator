#include "cache.hpp"
#include "protocol.hpp"
#include <memory>

using namespace std;

class MESIProtocol : public Protocol {
  public:
    shared_ptr<Request> onLoad(int pid, unsigned int address, shared_ptr<Bus> bus,
                               shared_ptr<Cache> cache) override;
    shared_ptr<Request> onStore(int pid, unsigned int address, shared_ptr<Bus> bus,
                                shared_ptr<Cache> cache) override;
};
