#pragma once

#include <cstdint>
#include "./RecordPointer.h"

#pragma pack(push, 1)

struct KeyMeta {
    bool active = false;
    RecordPointer rp;

    KeyMeta() = default;
    KeyMeta(RecordPointer& rp) : rp(rp) {};
    KeyMeta(RecordPointer& rp, bool active) : rp(rp), active(active) {};
};

#pragma pack(pop)