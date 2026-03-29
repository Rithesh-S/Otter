#pragma once

#include <cstdint>

#include "../../../OFS/DataNode/DataNode.h"
#include "../../data/TransactionStatus.h"
#include "../../../Index/data/RecordPointer.h"

#pragma pack(push, 1)

struct LogEntry {
    uint32_t tx_id;
    TStatus action;
    RecordPointer rp;
    DataNode prevRecord;
    DataNode currRecord;
    char padding[228];
    uint32_t crc; 

    LogEntry() = default;
    LogEntry(uint32_t tx_id, TStatus action, RecordPointer rp ,DataNode prev, DataNode curr) :
        tx_id(tx_id), action(action), rp(rp), prevRecord(prev), currRecord(curr) { crc = 0; }
    LogEntry(uint32_t tx_id, TStatus action) :
        tx_id(tx_id), action(action) { crc = 0; }
    
    void putCRC(uint32_t crc) { crc = crc; }
};

#pragma pack(pop)

static_assert(sizeof(LogEntry) == 512, "LogEntry size must be exactly 512 bytes!");