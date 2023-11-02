#include "cache_set.hpp"

CacheSet::CacheSet(size_t maxSize) {
    this->maxSize = maxSize;
    this->currentSize = 0;
}

bool CacheSet::checkCacheLine(size_t tag) {
    // TODO: there is a difference between read and write
    // iterate through the link list to check if it exist
    // return std::find(cacheSet.begin(), cacheSet.end(),
    // CacheLine(tag,0)) != cacheSet.end();
    return std::find_if(this->cacheSet.begin(), this->cacheSet.end(), [tag](const CacheLine &cl) {
               return cl.tag == tag;
           }) != this->cacheSet.end();
}

void CacheSet::invalidateCacheLine(size_t tag) {
    // invalidate the cacheline and remove out of the linked list
    auto it = std::find_if(this->cacheSet.begin(), this->cacheSet.end(),
                           [tag](const CacheLine &cl) { return cl.tag == tag; });

    if (it != this->cacheSet.end()) {
        this->cacheSet.erase(it);
    }
}

void CacheSet::addCacheLine(size_t tag, State state) {
    // create a cacheline class
    // add it to the back of the linked list if the size is less than max size
    if (cacheSet.size() == maxSize) {
        cacheSet.pop_front();
    }
    // else delete the LRU cache and add it to the back
    CacheLine cl = CacheLine(tag, state);
    // add it to linked list
    cacheSet.push_back(cl);
}

bool CacheSet::readCacheLine(size_t tag) {
    // remove from the current item and add it to the back fo the list
    auto it = std::find_if(this->cacheSet.begin(), this->cacheSet.end(),
                           [tag](const CacheLine &cl) { return cl.tag == tag; });

    if (it != this->cacheSet.end()) {
        this->cacheSet.splice(this->cacheSet.end(), this->cacheSet, it);
        return true;
    }

    return false;
}

bool CacheSet::updateCacheLine(size_t tag, State state) {
    // update the cacheline
    // need to pop out the cache and then update it then add it to the end of the list
    for (auto it = this->cacheSet.begin(); it != this->cacheSet.end(); ++it) {
        if (it->tag == tag) {
            // Update the state of the cache line
            it->state = state;

            // Move the updated cache line to the end of the list
            this->cacheSet.splice(this->cacheSet.end(), this->cacheSet, it);

            // Cache line updated successfully
            return true;
        }
    }

    // Cache line not found
    return false;
}

State CacheSet::checkCacheLineState(size_t tag) {
    for (auto &cacheLine : cacheSet) {
        if (cacheLine.tag == tag) {
            return cacheLine.getState();
        }
    }
    return I; // this will be similar to invalid state
}

void CacheSet::setCacheLineState(size_t tag, State state) {
    for (auto &cacheLine : cacheSet) {
        if (cacheLine.tag == tag) {
            cacheLine.setState(state);
        }
    }
}

bool CacheSet::checkCacheSetFull() { return cacheSet.size() == maxSize; }

bool CacheSet::isEmpty() { return cacheSet.empty(); }

CacheLine CacheSet::getFirst() { return cacheSet.front(); }
