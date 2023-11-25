#pragma once

#include "cache_set.hpp"
#include "state.hpp"
#include <cstddef>
#include <iostream>
#include <list>
#include <vector>

using namespace std;
class CacheSet;
class Cache {
  public:
    // deals with the LRU
    unsigned int associativity;
    unsigned int blockSize;
    unsigned int cacheSize;

    unsigned int offsetBits;
    unsigned int setIndexBits;
    unsigned int tagBits;

    unsigned int offsetMask;
    unsigned int setIndexMask;

    std::vector<CacheSet> cache;
    // the cache is being split into tag, index and offset
    // offset --> blockSize
    // index --> location of the block
    // tag --> is the associated memory stored in the cache
  public:
    Cache(unsigned int cacheSize, unsigned int associativity, unsigned int blockSize);

    unsigned int getIndex(unsigned int address);

    unsigned int getTag(unsigned int address);

    unsigned int getIndexWithTag(unsigned int address);

    unsigned int reverseGeneralAddress(unsigned int tag, unsigned int index);

    bool checkCacheLine(unsigned int address);

    bool invalidateCacheLine(unsigned int address);

    void addCacheLine(unsigned int address, State state);

    bool readCacheLine(unsigned int address);

    bool updateCacheLine(unsigned int address, State state);

    State getCacheLineState(unsigned int address);

    bool setCacheLineState(unsigned int address, State state);

    State getLRUCacheLineState(unsigned int address);

    bool checkCacheLineFull(unsigned int address);

    unsigned int getLRUCacheLineAddress(unsigned int address);
};
