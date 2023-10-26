#include "Cache.hpp"
Cache::Cache(unsigned int cacheSize, unsigned int associativity, unsigned int blockSize){
    this->associativity = associativity;
    this->blockSize = blockSize;
    this->cache = std::vector<CacheSet>(cacheSize/(associativity*blockSize), CacheSet(associativity));
    this->cacheSize = cacheSize/(associativity*blockSize); //TODO: rename this variable
}

unsigned int Cache::getIndex(unsigned int address) {
    // this returns the index of the memory
    return (address/blockSize) % cacheSize;
}

unsigned int Cache::getTag(unsigned int address) {
    return (address/blockSize) / cacheSize;
}

bool Cache::checkCacheLine(unsigned int address){
    // get the respective address
    // loop inside the cacheset list to find cacheline
    unsigned int index = getIndex(address);
    unsigned int tag = getTag(address);
    return cache[index].checkCacheLine(tag);
}

void Cache::invalidateCacheLine(unsigned int address){
    // performs a checkCacheLine
    // performs a invalidateCacheLine
    unsigned int index = getIndex(address);
    unsigned int tag = getTag(address);
    cache[index].invalidateCacheLine(tag);
}


void Cache::addCacheLine(unsigned int address, unsigned int state){
    // performs a addCacheLine
    unsigned int index = getIndex(address);
    unsigned int tag = getTag(address);
    cache[index].addCacheLine(tag,state);
}

bool Cache::readCacheLine(unsigned int address){
    // checkCacheLine
    // readCacheLine
    unsigned int index = getIndex(address);
    unsigned int tag = getTag(address);
    return cache[index].readCacheLine(tag);
}

bool Cache::updateCacheLine(unsigned int address,unsigned int state){
    // checkCacheLine
    unsigned int index = getIndex(address);
    unsigned int tag = getTag(address);
    return cache[index].updateCacheLine(tag,state);
}