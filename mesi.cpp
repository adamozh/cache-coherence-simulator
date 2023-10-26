#include "mesi.hpp"

void MESIProtocol::onLoad(unsigned int address, shared_ptr<Bus> bus, shared_ptr<Cache> cache) {
    // MESI-specific logic
    /*
    if hit -> return
    else -> create BusRd request
    */
}

void MESIProtocol::onStore(unsigned int address, shared_ptr<Bus> bus, shared_ptr<Cache> cache) {
    // MESI-specific logic
    /*
    if hit ->
        if E -> change to M, return
        else -> return
    else ->
        create BusRdX request

    */
}