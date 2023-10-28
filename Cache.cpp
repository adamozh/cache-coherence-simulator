#include "Cache.hpp"
Cache::Cache(size_t cacheSize, size_t associativity, size_t blockSize){
    this->associativity = associativity;
    this->blockSize = blockSize;
    this->cache = std::vector<CacheSet>(cacheSize/(associativity*blockSize), CacheSet(associativity));
    this->cacheSize = cacheSize/(associativity*blockSize); //TODO: rename this variable
}

size_t Cache::getIndex(size_t address) {
    // this returns the index of the memory
    return (address/blockSize) % cacheSize;
}

size_t Cache::getTag(size_t address) {
    return (address/(blockSize*cacheSize));
}

bool Cache::checkCacheLine(size_t address){
    // get the respective address
    // loop inside the cacheset list to find cacheline
    size_t index = getIndex(address);
    size_t tag = getTag(address);
    return cache[index].checkCacheLine(tag);
}

void Cache::invalidateCacheLine(size_t address){
    // performs a checkCacheLine
    // performs a invalidateCacheLine
    size_t index = getIndex(address);
    size_t tag = getTag(address);
    cache[index].invalidateCacheLine(tag);
}


void Cache::addCacheLine(size_t address, size_t state){
    // performs a addCacheLine
    size_t index = getIndex(address);
    size_t tag = getTag(address);
    cache[index].addCacheLine(tag,state);
}

bool Cache::readCacheLine(size_t address){
    // checkCacheLine
    // readCacheLine
    size_t index = getIndex(address);
    size_t tag = getTag(address);
    return cache[index].readCacheLine(tag);
}

bool Cache::updateCacheLine(size_t address,size_t state){
    // checkCacheLine
    size_t index = getIndex(address);
    size_t tag = getTag(address);
    return cache[index].updateCacheLine(tag,state);
}