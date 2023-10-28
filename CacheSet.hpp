
#ifndef CACHESET_H
#define CACHESET_H

#include "CacheLine.hpp"
#include "Cache.hpp"
#include <cstddef>
#include <list>
#include <algorithm>    
class Cache;

class CacheSet {
    public:
        size_t maxSize;
        size_t currentSize;
        std::list<CacheLine> cacheSet;
    public:
        CacheSet(size_t maxSize);

        bool checkCacheLine(size_t tag);

        void invalidateCacheLine(size_t tag);

        void addCacheLine(size_t tag,size_t state);

        bool readCacheLine(size_t tag);

        bool updateCacheLine(size_t tag, size_t state);
        
    friend class Cache;
};

#endif // CACHESET_H