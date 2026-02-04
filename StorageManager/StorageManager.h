#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <memory>
#include <iostream>
#include <filesystem>
#include <vector>
#include "../Index/BTree/BTree.h"
#include "../OFS/DataNode/DataNode.h"
#include "../OFS/Buffer/Buffer.h"
#include "../OFS/WAL/WAL.h"
#include "./LRUCache/LRUCache.h"
#include "./InsertionQueue/InsertionQueue.h"

class StorageManager {
    private:
        static uint32_t index;
        static const uint8_t length;
        static const size_t cacheSize;

        const std::string basepath = "OFS";
        const std::string basepathTree = "Index";
        const std::string basepathSM = "StorageManager";
        const std::string metaDataPath = basepath + "/Buffer/config/metadata.conf";
        const std::string treeIndexPath = basepathTree + "/bin/index.bin";
        const std::string walBinPath = basepath + "/WAL/bin/WALFrame.bin";
        const std::string iQueueBinPath = basepathSM + "/InsertionQueue/bin/DQueue.bin";

        static std::unique_ptr<BTree> tree;
        static std::unique_ptr<Buffer> buffer;
        static std::unique_ptr<WAL> wal;
        static std::unique_ptr<LRU> lruCache;
        static std::unique_ptr<InsertionQueue> iQueue;

        void writeRecord(std::vector<DataNode> walBuf);
        void overWriteRecord(uint32_t file_id, uint64_t offset, DataNode &node);

    public:
        StorageManager();
        ~StorageManager();

        void loadMetaData();
        void saveMetaData();

        std::string readRecord(uint32_t id);
        void writeRecord(std::ifstream &file);
        void writeRecord(uint32_t id, std::string msg);
        void updateRecord(uint32_t id, std::string msg);
        void deleteRecord(uint32_t id);

        uint32_t getCurrentBinIndex();
        uint32_t getNewIndexForBinFlush();
        std::string getWALBinPath();
        std::string getBTreeIndexPath();
        std::string getInsertionQueueBinPath();
        std::string getFilePathByIndex(uint32_t index);
        std::fstream* getFileByIndex(uint32_t index);
        std::pair<RecordPointer, std::fstream*> getInsertionPosAndFile();

        friend class WAL;
        friend class Buffer;
};