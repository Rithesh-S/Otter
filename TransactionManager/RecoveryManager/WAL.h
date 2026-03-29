#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <filesystem>
#include "./data/LogEntry.h"
#include "./data/WALHeader.h"
#include "../../OFS/DataNode/DataNode.h"
#include "../../Index/data/RecordPointer.h"

class WAL {
    private:
        static const uint16_t magic = 0xACE;
        static const uint8_t NODE_SIZE = 128;
        static const uint32_t key = 0xEDB88320;
        const std::string binPath;
        
        uint32_t crc32_table[256] = {0};
        
        std::fstream file;
        
        void init();
        bool verifyCRC(LogEntry& log);
        
        WALHeader seekHeader();
        void generateCRC32Table();

        size_t getWalLogFileSize();
        void writeWAL(LogEntry& log);
        
        uint32_t generateCRC(const void* data, size_t length);
        
    public:
        WAL(std::string binPath);
        ~WAL();
        
        bool isFull();
        bool isEmpty();
        void clearFile();
        size_t logCountInWal();

        void logData(uint32_t tx_id, RecordPointer rp, TStatus action, DataNode prev, DataNode curr);
        void logTransaction(uint32_t tx_id, TStatus action);
        bool readWAL(std::vector<LogEntry>& entries);
};
