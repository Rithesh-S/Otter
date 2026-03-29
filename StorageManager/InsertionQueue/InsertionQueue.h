#pragma once

#include <queue>
#include <string>
#include <fstream>
#include <cstdint>
#include "../../Index/data/RecordPointer.h"

class StorageManager;

class InsertionQueue {
    private:
        static const size_t RECORD_COUNT = 30;
        static std::queue<RecordPointer> insertionQueue;

        const std::string binPath = "./StorageManager/InsertionQueue/bin/DQueue.bin";

        std::fstream file;
        StorageManager* storageManager;

        void loadBinData();
        void saveBinData();
        void refillQueue(std::pair<uint16_t, uint16_t> indexPair);

    public:
        InsertionQueue(StorageManager* sm);
        ~InsertionQueue();
        
        RecordPointer getRecordPointer();
        void putRecordPointer(RecordPointer rp);
};