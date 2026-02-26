#pragma once

#include <list>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include "./data/Cache.h"

class StorageManager;

class LRU {
    private:
        size_t size;
        std::list<Cache> lruList;
        std::unordered_map<uint32_t, std::list<Cache>::iterator> lruMap;

        StorageManager* sm;

    public:
        LRU(StorageManager* sm, size_t size);
        ~LRU();

        std::fstream* getFileFromLRU(uint32_t file_id);
};