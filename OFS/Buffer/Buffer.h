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

class Buffer {
    private:
        static const uint16_t max_bytes = 1024;
        std::map<uint32_t, DataNode> records;
        size_t used_bytes = 0;
        StorageManager* storageManager;
        BTree* treeRef;

        bool saveTheNodesIntoBin(const std::string& filename, uint32_t file_id, std::map<uint32_t, DataNode>& records);

    public:
        Buffer(StorageManager* storageManager, BTree* treeRef);
        ~Buffer() = default;

        void writeData(uint32_t id, DataNode& record, size_t size);
        void flush();
        bool isFull();
        std::map<uint32_t, DataNode> readData();
        DataNode readData(uint32_t id);
        bool contains(uint32_t id);
};