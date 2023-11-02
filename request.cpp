#include "request.hpp"

Request::Request(int pid) : pid(pid){};

bool Request::isDone() { return done; };

void Request::decrement() { countdown--; };

void Request::setCountdown(unsigned int countdown) { this->countdown = countdown; }
