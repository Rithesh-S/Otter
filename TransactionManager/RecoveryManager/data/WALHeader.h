#pragma once

#include <cstdint>
#include <memory>

#pragma pack(push, 1)

struct WALHeader {
    uint16_t magic;
    uint16_t page_size;
    char padding[508];

    WALHeader() {
        magic = 0xACE;
        page_size = 8388608;           // 8MiB File
    }
    WALHeader(uint16_t magic, uint16_t page_size) : magic(magic), page_size(page_size) {};
};

#pragma pack(pop)

static_assert(sizeof(WALHeader) == 512, "WALHeader size must be exactly 512 bytes!");