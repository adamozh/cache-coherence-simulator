#pragma once

#include "transaction_type.hpp"

class Request {
  public:
    bool done = false;
    unsigned int countdown = 0;

    unsigned int pid;
    bool isModified;
    TransactionType type;
    unsigned int address;

    Request(int pid, TransactionType type) : pid(pid), type(type){};
};
