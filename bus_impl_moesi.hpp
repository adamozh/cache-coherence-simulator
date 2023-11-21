#pragma once

#ifndef BUS_IMPL_MOESI_H
#define BUS_IMPL_MOESI_H

#include "bus_impl.hpp"
#include "processor.hpp"
#include "request.hpp"
#include <memory>
#include <queue>
#include <vector>
#include <unordered_map>
#include <iostream>

class BusImplMOESI : public BusImpl {
    
private:
    void processBusRd(shared_ptr<Request> request);
    void processBusRdX(shared_ptr<Request> request);
public:
    //void executeCycle() override;
    BusImplMOESI(int wordsPerBlock) : BusImpl(wordsPerBlock){};
};  



#endif // BUS_IMPL_MOESI_H