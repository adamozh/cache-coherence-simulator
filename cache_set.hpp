
#ifndef CACHESET_H
#define CACHESET_H

#include "cache_line.hpp"
#include <algorithm>
#include <cstddef>
#include <list>

class CacheSet {
  public:
    size_t maxSize;
    size_t currentSize;
    std::list<CacheLine> cacheSet;

  public:
    CacheSet(size_t maxSize);

    bool checkCacheLine(size_t tag);

    void invalidateCacheLine(size_t tag);

    void addCacheLine(size_t tag, size_t state);

    bool readCacheLine(size_t tag);

    bool updateCacheLine(size_t tag, size_t state);

    friend class Cache;
};

#endif // CACHESET_H
