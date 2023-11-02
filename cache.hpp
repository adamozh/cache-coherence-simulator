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
    size_t associativity;
    size_t blockSize;
    size_t cacheSize;
    std::vector<CacheSet> cache;
    // the cache is being split into tag, index and offset
    // offset --> blockSize
    // index --> location of the block
    // tag --> is the associated memory stored in the cache
  public:
    Cache(size_t cacheSize, size_t associativity, size_t blockSize);

    size_t getIndex(size_t address);

    size_t getTag(size_t address);

    bool checkCacheLine(size_t address);

    void invalidateCacheLine(size_t address);

    void addCacheLine(size_t address, State state);

    bool readCacheLine(size_t address);

    bool updateCacheLine(size_t address, State state);

    State getCacheLineState(size_t address);

    void setCacheLineState(size_t address, State state);

    State getLRUCacheLineState(size_t address);
};
