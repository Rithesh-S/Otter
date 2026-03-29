#pragma once

#include <list>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include "./data/Cache.h"

class LRU {
    private:
        static const size_t CACHE_SIZE = 50;
        const std::string metaDataPath = "./StorageManager/LRUCache/config/metadata.conf";

        std::pair<uint32_t, uint32_t> indexPair; 
        std::fstream metaFile;

        std::list<Cache> lruList;
        std::unordered_map<uint32_t, std::list<Cache>::iterator> lruMap;

        void loadMetaData();
        void saveMetaData();
        std::string getFilePathByIndex(uint32_t index);
        
    public:
        LRU();
        ~LRU();
        
        std::pair<uint16_t, uint16_t> getNewIndex();
        std::fstream* getFileFromLRU(uint32_t file_id);
};