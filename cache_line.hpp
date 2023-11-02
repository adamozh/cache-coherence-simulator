#pragma once

#include "state.hpp"
#include <cstddef>

class CacheLine {
    State state; // The states currently will corresponds to 0,1,2,3
    // M -- 0, E -- 1, S -- 2, I -- 3 or M -- 0 E -- 1 Sc -- 2 Sm -- 3
    unsigned int tag;
    friend class CacheSet;

  public:
    CacheLine(unsigned int tag, State state);
    State getState();
    void setState(State state);
};
