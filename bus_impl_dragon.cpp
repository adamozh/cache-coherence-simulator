
// #include "bus_impl_dragon.hpp"

// bool BusImplDragon::checkCacheBlocked(unsigned int indexWithTag){
//     // Find the address in the unordered_map
//     auto it = cacheBlocked.find(indexWithTag);

//     // If address is found, return true, else return false
//     return it != cacheBlocked.end();
// }

// void BusImplDragon::addCacheBlocked(unsigned int indexWithTag, int pid){
//     // add the address in the unordered_map processorModifiedCache if does not exist
//     cacheBlocked.insert({indexWithTag, pid});
// }

// void BusImplDragon::removeCacheBlocked(unsigned int indexWithTag, int pid){
//     // remove the address in the unordered_map processorModifiedCache if it exist
//     auto it = cacheBlocked.find(indexWithTag);
//     if (it != cacheBlocked.end()) {
//         cacheBlocked.erase(it);
//     }
// }

// int BusImplDragon::getProcessorFromProcessorModifiedCache(unsigned int indexWithTag){
//     //get the value and return the key if it exist
//     auto it = processorModifiedCache.find(indexWithTag);

//     // Check if the key exists and return its associated value
//     if (it != processorModifiedCache.end()) {
//         return it->second; // Return the value associated with the key
//     }

//     // Return a default value or handle the case where the key doesn't exist
//     // Here, returning -1 assuming it's an invalid value
//     return -1; // Or any default value that signifies the key doesn't exist
// }

// void BusImplDragon::addProcessorModifiedCache(unsigned int indexWithTag, int pid){
//     processorModifiedCache.insert({indexWithTag, pid});
// }

// void BusImplDragon::removeProcessorModifiedCache(unsigned int indexWithTag, int pid){
//     auto it = processorModifiedCache.find(indexWithTag);

//     // Check if the key exists and remove it
//     if (it != processorModifiedCache.end()) {
//         processorModifiedCache.erase(it);
//     }
// }






