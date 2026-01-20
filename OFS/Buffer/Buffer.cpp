#include "Buffer.h"

void Buffer::writeData(uint32_t id, DataNode record, size_t size) {
    if(used_bytes + size > max_bytes) return;
    records[id] = record;
    used_bytes += size;
}

std::map<uint32_t, DataNode> Buffer::readData() { return records; }

bool Buffer::isFull() { return used_bytes == max_bytes; }

void Buffer::flush() {
    StorageManager storageManager;
    std::string filename = storageManager.getFileNameForBin();

    if(saveTheNodesIntoBin(filename, records)) {
        records.clear();
        used_bytes = 0;
    } else return;
}

bool Buffer::saveTheNodesIntoBin(const std::string filename, std::map<uint32_t, DataNode> records) {
    if(records.empty()) return false;

    std::ofstream outFile(filename, std::ios::binary | std::ios::out);

    if(!outFile) {
        std::cerr << "\033[31mERROR:Failed to create the bin file.\033[0m" << std::endl;
        return false;
    }

    for(auto [id, data]: records) {
        outFile.write(reinterpret_cast<const char*>(&data), sizeof(data));
    }

    outFile.close();
    return true;
}