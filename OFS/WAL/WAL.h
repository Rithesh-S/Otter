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

        static std::unique_ptr<WALFrame> walFrame;
        std::fstream file;
        StorageManager* storageManager;
        Buffer* bufferRef;

        void loadWALData();
        void saveNodesIntoWALBin();
        // uint32_t generateCRC();
        bool verifyCRC();
        std::vector<DataNode> readWAL();
        
    public:
        WAL(StorageManager* sm, Buffer* bufferRef);
        ~WAL();

        void writeWAL(DataNode& node);
};
