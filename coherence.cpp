#include <iostream>
#include <string>
#include <vector>
#include <list>


/**
    Generate the following output
    1. Overall Execution Cycle (outside)
    2. Number of compute cycles per core (processor)
    3. Number of load/store instruction per core (processor)
    4. Number of idle cycles (processor)
    5. Data cache iss rate for each core (processor)
    6. Amount of Data traffic in bytes on the bus (bus)
    7. Number of invalidations or updates on the bus (bus)
    8. Distribution of accesses to private data vs shared data
*/
// create a cacheline class
// create a cache class -- contains many cacheline class

class Cache;
class CacheLine;
class CacheSet;

class CacheLine {
    unsigned int state; // The states currently will corresponds to 0,1,2,3 
    // M -- 0, E -- 1, S -- 2, I -- 3 or M -- 0 E -- 1 Sc -- 2 Sm -- 3
    unsigned int tag;
    friend CacheSet;

    CacheLine(unsigned int tag, unsigned int state){
        this->tag = tag;
        this->state = state;
    }
};

class CacheSet {
    unsigned int maxSize;
    unsigned int currentSize;
    std::list<CacheLine> cacheSet;
    public:
        CacheSet(unsigned int maxSize){
            this->maxSize = maxSize;
            this->currentSize = 0;
        }

        bool checkCacheLine(unsigned int tag){
            // iterate through the link list to check if it exist
            // return std::find(cacheSet.begin(), cacheSet.end(),
            // CacheLine(tag,0)) != cacheSet.end();
            return std::find_if(cacheSet.begin(), cacheSet.end(),
            [tag](const CacheLine& cl) {
                return cl.tag == tag;
            }) != cacheSet.end();
        }

        void invalidateCacheLine(unsigned int tag){
            // invalidate the cacheline and remove out of the linked list
            auto it = std::find_if(cacheSet.begin(), cacheSet.end(),
            [tag](const CacheLine& cl) {
                return cl.tag == tag;
            }); 

            if (it != cacheSet.end()) {
                cacheSet.erase(it);
            }
        }

        void addCacheLine(unsigned int tag,unsigned int state){
            // create a Cache class
            // add it to the back of the linked list if the size if less than max size
            if (cacheSet.size() == maxSize) {
                cacheSet.pop_front();
            }
            // else delete the LRU cache and add it to the back
            CacheLine cl = CacheLine(tag,state);
            // add it to linked list
            cacheSet.push_back(cl);
        }

        bool readCacheLine(unsigned int tag){
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

        bool updateCacheLine(unsigned int tag, unsigned int state){
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
    friend Cache;
};


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
        Cache(unsigned int cacheSize, unsigned int associativity, unsigned int blockSize){
            this->associativity = associativity;
            this->blockSize = blockSize;
            this->cache = std::vector<CacheSet>(cacheSize/(associativity*blockSize), CacheSet(associativity));
            this->cacheSize = cacheSize/(associativity*blockSize);
        }

        unsigned int getIndex(unsigned int address) {
            // this returns the index of the memory
            return (address/blockSize) % cacheSize;
        }

        unsigned int getTag(unsigned int address) {
            return (address/blockSize) / cacheSize;
        }

        bool checkCacheLine(unsigned int address){
            // get the respective address
            // loop inside the cacheset list to find cacheline
            unsigned int index = getIndex(address);
            unsigned int tag = getTag(address);
            return cache[index].checkCacheLine(tag);
        }

        void invalidateCacheLine(unsigned int address){
            // performs a checkCacheLine
            // performs a invalidateCacheLine
            unsigned int index = getIndex(address);
            unsigned int tag = getTag(address);
            cache[index].invalidateCacheLine(tag);
        }

        void addCacheLine(unsigned int address, unsigned int state){
            // performs a addCacheLine
            unsigned int index = getIndex(address);
            unsigned int tag = getTag(address);
            cache[index].addCacheLine(tag,state);
        }

        bool readCacheLine(unsigned int address){
            // checkCacheLine
            // readCacheLine
            unsigned int index = getIndex(address);
            unsigned int tag = getTag(address);
            return cache[index].readCacheLine(tag);
        }

        bool updateCacheLine(unsigned int address,unsigned int state){
            // checkCacheLine
            unsigned int index = getIndex(address);
            unsigned int tag = getTag(address);
            return cache[index].updateCacheLine(tag,state);
        }

};





using namespace std;

int main(int argc, char* argv[]) {
    // Check if the correct number of arguments are provided
    if (argc != 6) {
        cerr << "Usage: ./coherence <protocol> <path-to-benchmark-trace> <cache-size> <associativity> <block-size>" << endl;
        return 1;
    }

    // Parse the command-line arguments
    string protocol = argv[1];
    string inputFilePath = argv[2];
    int cacheSize = stoi(argv[3]);
    int associativity = stoi(argv[4]);
    int blockSize = stoi(argv[5]);

    // Validate the protocol
    if (protocol != "MESI" && protocol != "Dragon") {
        cerr << "Invalid protocol. Choose either MESI or Dragon." << endl;
        return 1;
    }

    // Validate cache size, associativity, and block size (you can add more checks)
    if (cacheSize <= 0 || associativity <= 0 || blockSize <= 0) {
        cerr << "Cache size, associativity, and block size must be greater than 0." << endl;
        return 1;
    }

    // Display the parsed information (for debugging)
    cout << "Protocol: " << protocol << endl;
    cout << "Input File Path: " << inputFilePath << endl;
    cout << "Cache Size: " << cacheSize << endl;
    cout << "Associativity: " << associativity << endl;
    cout << "Block Size: " << blockSize << endl;

    // Your logic here for implementing the cache coherence protocols


    return 0;
}
