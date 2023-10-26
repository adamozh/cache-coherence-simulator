#include "CacheSet.hpp"

CacheSet::CacheSet(unsigned int maxSize){
    this->maxSize = maxSize;
    this->currentSize = 0;
}

bool CacheSet::checkCacheLine(unsigned int tag){  
    // TODO: there is a difference between read and write
    // iterate through the link list to check if it exist
    // return std::find(cacheSet.begin(), cacheSet.end(),
    // CacheLine(tag,0)) != cacheSet.end();
    return std::find_if(cacheSet.begin(), cacheSet.end(),
    [tag](const CacheLine& cl) {
        return cl.tag == tag;
    }) != cacheSet.end();
}

void CacheSet::invalidateCacheLine(unsigned int tag){
    // invalidate the cacheline and remove out of the linked list
    auto it = std::find_if(cacheSet.begin(), cacheSet.end(),
    [tag](const CacheLine& cl) {
        return cl.tag == tag;
    }); 

    if (it != cacheSet.end()) {
        cacheSet.erase(it);
    }
}

bool CacheSet::readCacheLine(unsigned int tag){
    // remove from the current item and add it to the back fo the list
    auto it = std::find_if(cacheSet.begin(), cacheSet.end(),
    [tag](const CacheLine& cl) {
        return cl.tag == tag;
    });



    if (it != cacheSet.end()) {
        cacheSet.splice(cacheSet.end(), cacheSet, it);
        return true;
    } 

    return false;
}


bool Cache::updateCacheLine(unsigned int tag, unsigned int state){
    // update the cacheline
    // need to pop out the cache and then update it then add it to the end of the list
    for (auto it = cacheSet.begin(); it != cacheSet.end(); ++it) {
        if (it->tag == tag) {
            // Update the state of the cache line
            it->state = state;

            // Move the updated cache line to the end of the list
            cacheSet.splice(cacheSet.end(), cacheSet, it);

            // Cache line updated successfully
            return true;
        }
    }

    // Cache line not found
    return false;
}