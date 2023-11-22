#include "cache.hpp"
#include <cmath>

Cache::Cache(unsigned int cacheSize, unsigned int associativity, unsigned int blockSize) {
    this->associativity = associativity;
    this->blockSize = blockSize;
    this->cacheSize = cacheSize;

    // Calculate the number of bits for offset, set index, and tag
    this->offsetBits = std::log2(blockSize);
    int numSets = cacheSize / (blockSize * associativity);
    this->setIndexBits = std::log2(numSets);
    this->tagBits = 32 - (offsetBits + setIndexBits);

    // Create masks for extracting set index and offset
    unsigned int offsetMask = (1 << offsetBits) - 1;
    unsigned int setIndexMask = ((1 << setIndexBits) - 1) << offsetBits;

    this->cache = std::vector<CacheSet>(numSets, CacheSet(associativity));
}

unsigned int Cache::getIndex(unsigned int address) {
    return (address & setIndexMask) >> offsetBits;
}

unsigned int Cache::getTag(unsigned int address) { return address >> (offsetBits + setIndexBits); }

unsigned int Cache::getIndexWithTag(unsigned int address) {
    // this returns the index of the memory
    return address / (blockSize);
}

unsigned int Cache::reverseGeneralAddress(unsigned int tag, unsigned int index) {
    return ((tag * (blockSize * cacheSize)) + (index)) * blockSize;
}

bool Cache::checkCacheLine(unsigned int address) {
    // get the respective address
    // loop inside the cacheset list to find cacheline
    unsigned int index = getIndex(address);
    unsigned int tag = getTag(address);
    return cache[index].checkCacheLine(tag);
}

void Cache::invalidateCacheLine(unsigned int address) {
    // performs a checkCacheLine
    // performs a invalidateCacheLine
    unsigned int index = getIndex(address);
    unsigned int tag = getTag(address);
    cache[index].invalidateCacheLine(tag);
}

void Cache::addCacheLine(unsigned int address, State state) {
    // performs a addCacheLine
    unsigned int index = getIndex(address);
    unsigned int tag = getTag(address);
    // this will only give the tag, need to add in the address and then pad with the block size
    cache[index].addCacheLine(tag, state);
}

bool Cache::readCacheLine(unsigned int address) {
    // checkCacheLine
    // readCacheLine
    unsigned int index = getIndex(address);
    unsigned int tag = getTag(address);
    return cache[index].readCacheLine(tag);
}

bool Cache::updateCacheLine(unsigned int address, State state) {
    // checkCacheLine
    unsigned int index = getIndex(address);
    unsigned int tag = getTag(address);
    return cache[index].updateCacheLine(tag, state);
}

State Cache::getCacheLineState(unsigned int address) {
    unsigned int index = getIndex(address);
    unsigned int tag = getTag(address);
    return cache[index].checkCacheLineState(tag);
}

bool Cache::setCacheLineState(unsigned int address, State state) {
    unsigned int index = getIndex(address);
    unsigned int tag = getTag(address);
    return cache[index].setCacheLineState(tag, state);
}

State Cache::getLRUCacheLineState(unsigned int address) {
    unsigned int index = getIndex(address);

    if (!cache[index].isEmpty()) {
        return cache[index].getFirst().getState();
    }
    return I;
}

bool Cache::checkCacheLineFull(unsigned int address) {
    unsigned int index = getIndex(address);
    return associativity == cache[index].size();
}

unsigned int Cache::getLRUCacheLineAddress(unsigned int address) {
    unsigned int index = getIndex(address);
    unsigned int tag = cache[index].getFirst().getTag();
    return reverseGeneralAddress(tag, index);
}
