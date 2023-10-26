#include "cache.hpp"

CacheLine::CacheLine(unsigned int tag, unsigned int state){
    this->tag = tag;
    this->state = state;
}