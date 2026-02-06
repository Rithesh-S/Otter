#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>
#include "../DataNode/DataNode.h"
#include "./data/WALFrame.h"

class StorageManager;
class Buffer;

class WAL {
    private:
        static const uint16_t magic = 0xACE;
        static const uint8_t nodeSize = 128;
        static const uint32_t key = 0xEDB88320;
        uint32_t crc32_table[256] = {0};
        
        static std::unique_ptr<WALFrame> walFrame;
        std::fstream file;
        StorageManager* storageManager;
        Buffer* bufferRef;
        
        void generateCRC32Table();
        void saveNodesIntoWALBin();
        uint32_t generateCRC(const void* data, size_t length);
        bool verifyCRC();
        std::vector<DataNode> readWAL();
        
    public:
        WAL(StorageManager* sm, Buffer* bufferRef, std::string binPath);
        ~WAL();
        
        void loadWALData();
        void walFrameClearAndSave();
        void writeWAL(DataNode& node);
};
