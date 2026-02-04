#pragma once

#include <iostream>
#include <unordered_map>
#include <list>
#include <memory>
#include <fstream>
#include <string>
#include "./data/Cache.h"

class StorageManager;

class LRU {
    private:
        std::list<Cache> lruList;
        std::unordered_map<uint32_t, std::list<Cache>::iterator> lruMap;
        size_t size;

        StorageManager* sm;

    public:
        LRU(StorageManager* sm, size_t size);
        ~LRU();
        std::fstream* getFileFromLRU(uint32_t file_id);
};