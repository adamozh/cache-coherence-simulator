#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

/**
 * Useless for now
 */
class Memory {
  private:
    unsigned int size;

    vector<int> mem; // actually doesn't need to hold anything

  public:
    Memory(unsigned int size) : mem(size, 0) {}

    int read(unsigned int address) {
        if (address > size) {
            throw out_of_range("index out of range: " + to_string(address));
        }
        return mem[address];
    }

    void write(unsigned int address, int value) {
        if (address > size) {
            throw out_of_range("index out of range: " + to_string(address));
        }
        mem[address] = value;
    }
};
