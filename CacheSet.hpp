#include "Cache.hpp"

class CacheSet {
    size_t maxSize;
    size_t currentSize;
    std::list<CacheLine> cacheSet;
    public:
        CacheSet(size_t maxSize){};

        bool checkCacheLine(size_t tag){};

        void invalidateCacheLine(size_t tag){};

        void addCacheLine(size_t tag,size_t state){};

        bool readCacheLine(size_t tag){};

        bool updateCacheLine(size_t tag, size_t state){};
        
    friend Cache;
};