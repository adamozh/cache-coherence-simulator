#include "cache_line.hpp"
#include "state.hpp"

CacheLine::CacheLine(unsigned int tag, State state) : tag(tag), state(state){};

State CacheLine::getState() { return state; };

unsigned int CacheLine::getTag() { return tag; };

void CacheLine::setState(State state) { this->state = state; }

