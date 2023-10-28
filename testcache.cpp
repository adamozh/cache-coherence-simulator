#include "Cache.hpp"
#include "CacheLine.hpp"
#include "CacheSet.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <cmath>
#include <cstddef>
using namespace std;
int main() {
    // test the cacheclasses
    size_t cacheSize = pow(2,9);
    size_t associativity = pow(2,2);
    size_t blockSize = pow(2,3);
    Cache c = Cache(cacheSize,associativity,blockSize);
    // get the index from the address
    // the cache consist of 3 parts
    // tag, index, word
    // the bits that is remained by word is based on the blocksize
    // the bits by index is dependent on cacheSize
    size_t exampleAddress = pow(2,32) - 1; 
    size_t index = c.getIndex(exampleAddress);
    cout << "this is the index of max address " << std::bitset<32>(index) << endl;
    size_t tag = c.getTag(exampleAddress);
    cout << "this is the tag of max address " << std::bitset<32>(tag) << endl;



    // test the update cache

    // test add
    size_t exampleAddress1 = pow(2,32) - 1;
    size_t exampleAddress2 = pow(2,32) - 1 - 2*blockSize;
    size_t exampleAddress3 = pow(2,32) - 1 - 3*blockSize;
    size_t exampleAddress4 = pow(2,32) - 1 - 4*blockSize;
    size_t exampleAddress5 = pow(2,32) - 1 - 5*blockSize;
    cout << c.checkCacheLine(exampleAddress1) << endl;
    c.addCacheLine(exampleAddress1,1);
    cout << c.checkCacheLine(exampleAddress1) << endl;
    c.addCacheLine(exampleAddress2,1);
    c.addCacheLine(exampleAddress2,1);
    c.addCacheLine(exampleAddress3,1);
    c.addCacheLine(exampleAddress4,1);
    c.addCacheLine(exampleAddress5,1);
    cout << "Check whether auto deletion works " << c.checkCacheLine(exampleAddress1) << endl;

    // check the length of the cache
    cout << "This is the cache current size " << c.cache[15].cacheSet.size() <<endl;

    cout << "This is the cache maxSize " << c.cache[15].maxSize << endl;
}