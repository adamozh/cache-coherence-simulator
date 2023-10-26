#include "Cache.hpp"

class CacheSet {
    unsigned int maxSize;
    unsigned int currentSize;
    std::list<CacheLine> cacheSet;
    public:
        CacheSet(unsigned int maxSize){};

        bool checkCacheLine(unsigned int tag){};

        void invalidateCacheLine(unsigned int tag){};

        void addCacheLine(unsigned int tag,unsigned int state){};

        bool readCacheLine(unsigned int tag){};

        bool updateCacheLine(unsigned int tag, unsigned int state){};
        
    friend Cache;
};