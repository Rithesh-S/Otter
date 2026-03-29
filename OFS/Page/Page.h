#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "./data/Slot.h"
#include "../DataNode/DataNode.h"

#pragma pack(push, 1)
class Page {
    private:
        static const uint16_t MAX_RECORDS = 30;

        // uint16_t page_no;
        uint16_t record_count;
        uint32_t checksum;

        // ----- Header(8 bytes) -----

        Slot slots[MAX_RECORDS];
        DataNode records[MAX_RECORDS];

        // ----- Body(240 + 3,840 bytes) -----
        
        uint8_t padding[8];

        void clean();
        bool hasDeletion();
        int findRecordIndex(uint32_t target_id);
        
    public:
        uint16_t page_no;
        Page() = default;
        Page(uint16_t page_no);
        
        bool isFull();
        bool contains(uint32_t id);
        
        bool removeData(uint32_t target_id);
        bool writeData(DataNode& record);
        bool readData(DataNode& record, uint32_t target_id);
};
#pragma pack(pop)

static_assert(sizeof(Page) == 4096, "Page size must be exactly 4096 bytes!");