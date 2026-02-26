#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>
#include "../DataNode/DataNode.h"
#include "./data/WALFrame.h"

class WAL {
    private:
        static const uint16_t magic = 0xACE;
        static const uint8_t nodeSize = 128;
        static const uint32_t key = 0xEDB88320;
        static const uint32_t MAX_RECORD_COUNT = 32;
        
        uint32_t crc32_table[256] = {0};
        
        std::fstream file;
        std::unique_ptr<WALFrame> walFrame;
        
        bool verifyCRC();
        void generateCRC32Table();
        void saveNodesIntoWALBin();
        uint32_t generateCRC(const void* data, size_t length);
        
    public:
        WAL(std::string binPath);
        ~WAL();
        
        bool isFull();
        void walFrameClearAndSave();
        void writeWAL(DataNode& node);
        std::vector<DataNode> readWAL();
};
