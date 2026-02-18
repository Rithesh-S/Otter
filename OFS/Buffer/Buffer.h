#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include "../DataNode/DataNode.h"

class StorageManager;
class BTree;
class Transaction;

class Buffer {
    private:
        static const uint16_t MAX_BYTES = 1024;

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
        void writeData(uint32_t id, DataNode& record, size_t size);
};