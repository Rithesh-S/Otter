#pragma once

#include <list>
#include <cstdint>
#include <unordered_map>
#include "./data/BufferFrame.h"

class StorageManager;

class BufferPool {
    private:
        static const size_t POOL_SIZE = 256;

        // Physical Order
        BufferFrame frames[POOL_SIZE];

        // Logical Order (moves indices)
        std::list<size_t> lruList;  // index of frame

        // Fast Lookup
        std::unordered_map<uint32_t, std::list<size_t>::iterator> lookUp; // file_id, page_no -> iterator of index

        // Contains the Emptied slots
        std::list<size_t> freeSlots;

        StorageManager* sm;

        bool isFull();
        void evict();
        size_t getFreeSlots();
        void addFreeSlots(size_t idx);
        uint32_t keyCombiner(uint16_t file_id, uint16_t page_no);

    public:
        BufferPool(StorageManager* sm);
        ~BufferPool();

        Page* getPage(uint16_t file_id, uint16_t page_no);
};