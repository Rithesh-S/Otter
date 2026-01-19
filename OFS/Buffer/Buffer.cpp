#include "Buffer.h"

void Buffer::writeData(uint32_t id, DataNode record, size_t size) {
    if(used_bytes + size > max_bytes) return;

    records.insert({id, record});
    used_bytes += size;

}

std::map<uint32_t, DataNode> Buffer::readData() {
    return records;
}