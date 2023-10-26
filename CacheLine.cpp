#include "cache.hpp"

CacheLine::CacheLine(size_t tag, size_t state){
    this->tag = tag;
    this->state = state;
}