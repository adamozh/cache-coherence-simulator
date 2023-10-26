class Cache {
    // deals with the LRU
    unsigned int associativity;
    unsigned int blockSize;
    unsigned int cacheSize;
    std::vector<CacheSet> cache;
    // the cache is being split into tag, index and offset
    // offset --> blockSize
    // index --> location of the block
    // tag --> is the associated memory stored in the cache
    public:
        Cache(unsigned int cacheSize, unsigned int associativity, unsigned int blockSize){};

        unsigned int getIndex(unsigned int address){};

        unsigned int getTag(unsigned int address){};

        bool checkCacheLine(unsigned int address){};

        void invalidateCacheLine(unsigned int address){};

        void addCacheLine(unsigned int address, unsigned int state){};

        bool readCacheLine(unsigned int address){};

        bool updateCacheLine(unsigned int address,unsigned int state){};
};