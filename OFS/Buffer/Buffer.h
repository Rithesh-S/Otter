#pragma once

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <fstream>
#include <iostream>
#include "../DataNode/DataNode.h"

class StorageManager;
class BTree;
class Transaction;

class Buffer {
    private:
        static const uint16_t MAX_BYTES = 4096;

        size_t used_bytes = 0;
        std::map<uint32_t, DataNode> records;

        BTree* treeRef;
        Transaction* transactionRef;
        StorageManager* storageManager;

        bool saveTheNodesIntoBin(std::map<uint32_t, DataNode>& records);
        
    public:
        Buffer(StorageManager* storageManager, BTree* treeRef, Transaction* transactionRef);
        ~Buffer() = default;
        
        void flush();
        bool isFull();
        bool contains(uint32_t id);
        
        void removeData(uint32_t id);
        DataNode readData(uint32_t id);
        std::map<uint32_t, DataNode> readData();
        void writeRecordsFromWal(std::vector<DataNode> walBuf);
        void writeData(uint32_t id, DataNode& record, size_t size);
};