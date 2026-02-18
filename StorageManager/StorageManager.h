#pragma once

#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "../Index/BTree/BTree.h"
#include "../OFS/DataNode/DataNode.h"
#include "../OFS/Buffer/Buffer.h"
#include "../OFS/WAL/WAL.h"
#include "./LRUCache/LRUCache.h"
#include "./InsertionQueue/InsertionQueue.h"
#include "../TransactionManager/Transaction.h"

class StorageManager {
    private:
        uint32_t index;
        static const uint8_t length;
        static const size_t cacheSize;
        std::fstream metaFile;

        const std::string basepath = "OFS";
        const std::string basepathTree = "Index";
        const std::string basepathSM = "StorageManager";
        const std::string treeIndexPath = basepathTree + "/bin/index.bin";
        const std::string walBinPath = basepath + "/WAL/bin/WALFrame.bin";
        const std::string metaDataPath = basepath + "/Buffer/config/metadata.conf";
        const std::string iQueueBinPath = basepathSM + "/InsertionQueue/bin/DQueue.bin";

        std::unique_ptr<WAL> wal;
        std::unique_ptr<BTree> tree;
        std::unique_ptr<LRU> lruCache;
        std::unique_ptr<Buffer> buffer;
        std::unique_ptr<InsertionQueue> iQueue;
        std::unique_ptr<Transaction> transaction;

        void writeRecord(std::vector<DataNode> walBuf);
        void overWriteRecord(uint32_t file_id, uint64_t offset, DataNode &node);

    public:
        StorageManager();
        ~StorageManager();

        void loadMetaData();
        void saveMetaData();

        bool deleteRecord(uint32_t id);
        bool writeRecord(uint32_t id, std::string msg);
        bool updateRecord(uint32_t id, std::string msg);
        std::pair<std::string, std::string> readRecord(uint32_t id);

        void walFrameClearAndSave();
        void recover();
        
        std::string getWALBinPath();
        std::string getBTreeIndexPath();
        std::string getInsertionQueueBinPath();
        
        uint32_t getCurrentBinIndex();
        uint32_t getNewIndexForBinFlush();
        std::fstream* getFileByIndex(uint32_t index);
        std::string getFilePathByIndex(uint32_t index);
        std::pair<RecordPointer, std::fstream*> getInsertionPosAndFile();

        friend class WAL;
        friend class Buffer;
};