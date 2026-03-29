#pragma once

#include <cstdint>

#pragma pack(push, 1)

struct RecordPointer {
    uint16_t file_id = 0xFFFF;
    uint16_t page_no = 0xFFFF;

    RecordPointer() = default;
    RecordPointer(uint16_t file_id, uint16_t page_no) : 
        file_id(file_id), page_no(page_no) {};
};

#pragma pack(pop)