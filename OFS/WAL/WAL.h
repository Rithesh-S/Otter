#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>
#include "../DataNode/DataNode.h"
#include "./data/WALFrame.h"

class StorageManager;

class WAL {
    private:
        const std::string walBinPath = "OFS/WAL/bin/WALFrame.bin";
        static const uint16_t magic = 0xACE;
        static const uint8_t nodeSize = 128;
        static std::unique_ptr<WALFrame> walFrame;
        std::fstream file;
        StorageManager* storageManager;

        void loadWALData();
        void saveNodesIntoWALBin();
        // uint32_t generateCRC();
        bool verifyCRC();
        void readWAL();
        
    public:
        WAL() = default;
        WAL(StorageManager* sm);
        ~WAL();

        void writeWAL(DataNode& node);
};
