
// #include "dragon.hpp"

// using namespace std;

// bool dragon_debug = true;

// CacheResultType DragonProtocol::onLoad(int pid,unsigned int address, shared_ptr<Bus> bus, shared_ptr<Cache> cache) {
//     // MESI-specific logic
//     /*
//     if hit -> return
//     else -> create BusRd request
//     */

//     // need to check for cache full also
//     // 1. check if the cache is present, if present check the state
//     // 1a. if it is sc, need to check if it is locked.
//     // 2. if the state is M or Sm --> cache hit and continue
//     //2a. if there is sm --> wiil need to "lock the other users from reading" 
//     // can be implemented as a queue
//     // 3. if the state is Sc, check if Sm is present
//     // and change the cache with Sm to Sc
//     State state = cache->getCacheLineState(address);
//     if (state == M || state == E || state == Sm) {
//         if (dragon_debug) cout << "M/E/Sm: load hit" << endl;
//         return CACHEHIT;
//     } else if (state == Sc){
//         // check if it is store in the dictionary, if is stored it will be stalled.
//         size_t indexWithTag = cache->getIndexWithTag(address);
//         bool isCacheBlocked = bus->checkCacheBlocked(indexWithTag);
//         //TODO: need to discuss with Adam

//         return CACHEBLOCKED;
//     } else if (state == I) {
//         if (dragon_debug) cout << "I: load miss" << endl;
//         shared_ptr<Request> busRdRequest = make_shared<Request>(pid, BusRd, address);
//         bus->pushRequestToBus(busRdRequest);
//         return CACHEMISS;
//     } else {
//         throw runtime_error("invalid state");
//     }
        

// }

// CacheResultType DragonProtocol::onStore(int pid,unsigned int address, shared_ptr<Bus> bus, shared_ptr<Cache> cache) {
//     State state = cache->getCacheLineState(address);
//     if (state == M || state == Sm) {
//         if (dragon_debug) cout << "M/E/Sm: load hit" << endl;
//         return CACHEHIT;
//     } else if (state == E){
//         // change state to M
//         cache->updateCacheLine(address,M);
//         return CACHEHIT;
//     } else if (state == Sc){
//         // check if it is store in the dictionary, if is stored it will be stalled.
//         size_t indexWithTag = cache->getIndexWithTag(address);
//         bool isCacheBlocked = bus->checkCacheBlocked(indexWithTag);
//         if (!isCacheBlocked){
//             // change the Sm to Sc on the other processor 
//             int old_pid = bus->getProcessorFromProcessorModifiedCache(indexWithTag);
//             bus->getProcessor(old_pid)->setState(address,Sc);
//             // change the data in the bus unordered map
//             bus->updateProcessorModifiedCache(indexWithTag,pid);
//             // change add the address into the cacheBlocked data -- and will be removed by bus
//             bus->addCacheBlocked(indexWithTag,pid);
//             return CACHEHIT;
//         }
//         else{
//             // ASKADAM: cannot do anything -- perform stalling
//             int needHelp = 1;
//         }

//         return CACHEBLOCKED;
//     } else if (state == I) {
//         // will also need to check whether other processors have the cache
//         if (dragon_debug) cout << "I: load miss" << endl;
//         shared_ptr<Request> busRdRequest = make_shared<Request>(pid, BusRd, address);
//         bus->pushRequestToBus(busRdRequest);
//         return CACHEMISS;
//     } else {
//         throw runtime_error("invalid state");
//     }
    
// }