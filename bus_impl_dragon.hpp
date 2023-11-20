// #pragma once

// #ifndef BUS_IMPL_DRAGON_H
// #define BUS_IMPL_DRAGON_H

// #include "bus_impl.hpp"
// #include "processor.hpp"
// #include "request.hpp"
// #include <memory>
// #include <queue>
// #include <vector>
// #include <unordered_map>

// class BusImplDragon : public BusImpl {
//     // Additional functionalities or modifications here
// private: // might want to change it to protected so that the child class can access the variables
//     vector<shared_ptr<Processor>> processors;
//     shared_ptr<Request> currReq = nullptr;
//     vector<Request> memRequests;
//     queue<shared_ptr<Request>> busQueue;
//     // additional features
//     unordered_map<unsigned int, int> cacheBlocked;// check whether memory has been modified
//     unordered_map<unsigned int, int> processorModifiedCache;
    

// public:
//     //void executeCycle() override;
//     bool checkCacheBlocked(unsigned int indexWithTag);
//     void addCacheBlocked(unsigned int indexWithTag, int pid);
//     void removeCacheBlocked(unsigned int indexWithTag, int pid);
//     int getProcessorFromProcessorModifiedCache(unsigned int indexWithTag);
//     void addProcessorModifiedCache(unsigned int indexWithTag, int pid);
//     void removeProcessorModifiedCache(unsigned int indexWithTag, int pid);
//     void updateProcessorModifiedCache(unsigned int indexWithTag,int pid);
// };



// #endif // BUS_IMPL_DRAGON_H