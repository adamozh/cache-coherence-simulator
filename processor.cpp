#include <iostream>
#include <vector>

using namespace std;

enum ProcessorState { FREE, LOAD, STORE, NON_MEMORY };

class Processor {
    unsigned int id;
    ProcessorState state;
    unsigned int timeLeft;

    Cache l1Data;

  public:
    Processor(unsigned int id, unsigned int cacheSize, unsigned int associativity,
              unsigned int blockSize)
        : id(id), l1Data(cacheSize, associativity, blockSize) {}

    void exeute(unsigned int type, int value) {
        switch (type) {
        case 0: // load
            // first do a read to the current cache line
            bool isHit = l1Data.checkCacheLine(value);
            // if true, means cache hit. can immediately run the next instruction
            // if false, means cache miss. stall
            if (isHit) {
                state = FREE;
                timeLeft = 0;
            } else {
                state = LOAD;
                timeLeft = 100;
            }
            break;
        case 1: // store
            // first do a write to the current cache line
            bool isHit = l1Data.checkCacheLine(value);
            if (isHit) {
                state = FREE;
                timeLeft = 0;
            } else {
                state = LOAD;
                timeLeft = 100;
            }
            break;
        case 2: // non-memory instructions
            state = NON_MEMORY;
            timeLeft = value;
            break;
        default:
            throw logic_error("invalid instruction type");
        }
    }
};