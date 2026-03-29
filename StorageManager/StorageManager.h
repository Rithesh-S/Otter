#pragma once

#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "./LRUCache/LRUCache.h"
#include "../Index/BTree/BTree.h"
#include "../OFS/DataNode/DataNode.h"
#include "../OFS/BufferPool/BufferPool.h"
#include "./InsertionQueue/InsertionQueue.h"
#include "./PersistenceManager/PersistenceManager.h"
// #include "../TransactionManager/Transaction.h"

class StorageManager {
    private:
        static const uint8_t DATA_LENGTH = 124;
        static const size_t PAGE_SIZE = 4096;

        std::unique_ptr<BTree> tree;
        std::unique_ptr<LRU> lruCache;
        std::unique_ptr<BufferPool> bPool;
        std::unique_ptr<InsertionQueue> iQueue;
        std::unique_ptr<PersistenceManager> pManager;
        // std::unique_ptr<Transaction> transaction;

        void overWriteRecord(uint32_t file_id, uint64_t offset, DataNode &node);
        void writePageIntoBin(uint16_t file_id, uint16_t page_no, Page& page);

    public:
        StorageManager();

        bool deleteRecord(uint32_t id);
        bool writeRecord(uint32_t id, std::string msg);
        bool updateRecord(uint32_t id, std::string msg);
        std::pair<std::string, std::string> readRecord(uint32_t id);

        void init();
        
        void persistBatch();
        uint8_t getCurrentBatchId();
        bool isBatchPersisted(uint8_t batch_id);
        void writePageIntoBin(BufferFrame& frame);
        std::fstream* getFileByIndex(uint32_t index);
        std::pair<uint16_t, uint16_t> getNewIndexForBinFlush();
        std::pair<RecordPointer, std::fstream*> getInsertionPosAndFile();
        bool readPageFromBin(uint16_t file_id, uint16_t page_no, Page& page);
};