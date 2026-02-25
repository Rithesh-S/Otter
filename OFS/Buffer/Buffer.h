#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include "../DataNode/DataNode.h"

class StorageManager;
class BTree;
class Transaction;

class Buffer {
    private:
        static const uint16_t MAX_BYTES = 4096;
        static const uint16_t VEC_SIZE = MAX_BYTES / sizeof(DataNode);

        size_t used_bytes = 0;
        std::vector<DataNode> records;

        BTree* treeRef;
        Transaction* transactionRef;
        StorageManager* storageManager;

        bool saveTheNodesIntoBin(std::vector<DataNode>& records);
        
    public:
        Buffer(StorageManager* storageManager, BTree* treeRef, Transaction* transactionRef);
        ~Buffer() = default;
        
        void flush();
        bool isFull();
        bool contains(uint32_t id);
        
        void removeData(uint32_t id);
        DataNode readData(uint32_t id);
        std::vector<DataNode> readData();
        void writeRecordsFromWal(std::vector<DataNode> walBuf);
        void writeData(DataNode& record);
};