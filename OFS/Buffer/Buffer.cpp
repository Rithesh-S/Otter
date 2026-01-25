#include "Buffer.h"

void Buffer::writeData(uint32_t id, DataNode& record, size_t size) {
    if(used_bytes + size > max_bytes) return;
    records[id] = record;
    used_bytes += size;
}

std::map<uint32_t, DataNode> Buffer::readData() { return records; }

DataNode Buffer::readData(uint32_t id) { return records[id]; }

bool Buffer::isFull() { return used_bytes == max_bytes; }

bool Buffer::contains(uint32_t id) { return records.find(id) != records.end(); }

void Buffer::flush() {
    StorageManager storageManager;
    std::string filename = storageManager.getFileNameForBin();
    uint32_t file_id = storageManager.getCurrentBinIndex();

    if(saveTheNodesIntoBin(filename, file_id, records)) {
        records.clear();
        used_bytes = 0;
    } else return;
}

bool Buffer::saveTheNodesIntoBin(const std::string& filename, uint32_t file_id, std::map<uint32_t, DataNode>& records) {
    if(records.empty()) return false;

    std::ofstream outFile(filename, std::ios::binary | std::ios::out);
    BTree tree(indexPath);

    if(!outFile) {
        std::cerr << "\033[31mERROR:Failed to create the bin file.\033[0m" << std::endl;
        return false;
    }

    for(auto [id, data]: records) {
        uint64_t currPos = outFile.tellp();
        outFile.write(reinterpret_cast<const char*>(&data), sizeof(data));
        tree.insert(id, file_id ,currPos);
    }

    outFile.close();
    return true;
}