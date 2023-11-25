#pragma once

#ifndef BUS_IMPL_MOESI_H
#define BUS_IMPL_MOESI_H

#include "bus_impl.hpp"
#include "processor.hpp"
#include "request.hpp"
#include <iostream>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

class BusImplMOESI : public BusImpl {

  protected:
    void processBusRd(shared_ptr<Request> request) override;
    void processBusRdX(shared_ptr<Request> request) override;

  public:
    // void executeCycle() override;
    BusImplMOESI(int wordsPerBlock) : BusImpl(wordsPerBlock){};
};

#endif // BUS_IMPL_MOESI_H
