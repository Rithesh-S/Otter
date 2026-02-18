#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>
#include "../DataNode/DataNode.h"
#include "./data/WALFrame.h"

class StorageManager;
class Transaction;
class Buffer;

class WAL {
    private:
        static const uint16_t magic = 0xACE;
        static const uint8_t nodeSize = 128;
        static const uint32_t key = 0xEDB88320;
        
        uint32_t crc32_table[256] = {0};
        
        std::fstream file;
        Buffer* bufferRef;
        Transaction* transactionRef;
        StorageManager* storageManager;
        std::unique_ptr<WALFrame> walFrame;
        
        bool verifyCRC();
        void generateCRC32Table();
        void saveNodesIntoWALBin();
        std::vector<DataNode> readWAL();
        uint32_t generateCRC(const void* data, size_t length);
        
    public:
        WAL(StorageManager* sm, Buffer* bufferRef, Transaction* transactionRef, std::string binPath);
        ~WAL();
        
        void loadWALData();
        void walFrameClearAndSave();
        void writeWAL(DataNode& node);
};
