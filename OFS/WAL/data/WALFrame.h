#pragma once

#include <cstdint>
#include <memory>

#pragma pack(push, 1)

struct WALFrame {
    uint16_t magic;
    uint32_t crc;
    uint32_t record_count; 
    char node[1024];

    WALFrame() {
        magic = 0xACE;
        crc = 0;
        record_count = 0;
        for(size_t i = 0; i < 1024; i++) node[i] = '0';
    }
    ~WALFrame() = default;
};

#pragma pack(pop)

static_assert(sizeof(WALFrame) == 1034, "WALFrame size must be exactly 1034 bytes!");