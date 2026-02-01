#pragma once

#include <memory>

#pragma pack(push, 1)

struct RecordPointer {
    uint32_t file_id;
    uint64_t offset;
};

#pragma push(pop)