#pragma once

#include <cstdint>

#pragma pack(push, 1)

struct Slot {
    uint32_t record_id;     // Record's ID
    uint8_t index;          // Index of Physical Block
    uint8_t active;         // Does the ID exists
    uint8_t padding[2];
};

#pragma pack(pop)

static_assert(sizeof(Slot) == 8, "Slot size must be exactly 8 bytes!");
