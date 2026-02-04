#pragma once

#include <queue>
#include <fstream>
#include <cstdint>
#include "../../Index/data/RecordPointer.h"

class StorageManager;

class InsertionQueue {
    private:
        static const size_t bufferSize = 1024;
        static const size_t dataNodeSize = 128;
        static std::queue<RecordPointer> insertionQueue;

        StorageManager* storageManager;
        std::fstream file;
        void refillQueue(uint32_t file_id);
        void loadBinData();
        void saveBinData();

    public:
        InsertionQueue(StorageManager* sm, std::string binPath);
        ~InsertionQueue();
        RecordPointer getRecordPointer();
        void putRecordPointer(RecordPointer rp);
};