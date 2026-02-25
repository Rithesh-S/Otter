#pragma once

#include <queue>
#include <fstream>
#include <cstdint>
#include "../../Index/data/RecordPointer.h"

class StorageManager;

class InsertionQueue {
    private:
        static const size_t bufferSize = 4096;
        static const size_t dataNodeSize = 128;
        static std::queue<RecordPointer> insertionQueue;

        std::fstream file;
        StorageManager* storageManager;

        void loadBinData();
        void saveBinData();
        void refillQueue(uint32_t file_id);

    public:
        InsertionQueue(StorageManager* sm, std::string binPath);
        ~InsertionQueue();
        
        RecordPointer getRecordPointer();
        void putRecordPointer(RecordPointer rp);
};