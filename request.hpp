#pragma once

#include "transaction_type.hpp"

class Request {
  public:
    bool done = false;
    unsigned int countdown = 0;

    unsigned int pid;
    bool isToMemOrCache = false;
    TransactionType type;
    unsigned int address;

    Request(int pid, TransactionType type, unsigned int address)
        : pid(pid), type(type), address(address){};
};
