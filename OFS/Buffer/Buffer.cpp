#include "Buffer.h"
#include "../../StorageManager/StorageManager.h"
#include "../../TransactionManager/Transaction.h"
#include "../../Index/BTree/BTree.h"

Buffer::Buffer(StorageManager *storageManager, BTree *treeRef, Transaction* transactionRef) : 
    storageManager(storageManager), treeRef(treeRef), transactionRef(transactionRef) {}

std::map<uint32_t, DataNode> Buffer::readData() { return records; }

void Buffer::removeData(uint32_t id) { records.erase(id); }

DataNode Buffer::readData(uint32_t id) { return records.at(id); }

bool Buffer::isFull() { return used_bytes == MAX_BYTES; }

bool Buffer::contains(uint32_t id) { return records.find(id) != records.end(); }
    
void Buffer::writeData(uint32_t id, DataNode &record, size_t size) {
    if (used_bytes + size > MAX_BYTES) return;
    records[id] = record;
    used_bytes += size;
}

void Buffer::writeRecordsFromWal(std::vector<DataNode> walBuf) {
    char data[124] = {0};
    for (auto &node : walBuf) {
        uint32_t id = node.getData().first;
        if(node.getData().second == data) {
            removeData(id);
            continue;
        } 
        writeData(id, node, sizeof(node));
    }
}

void Buffer::flush() {
    static bool isAlreadyFlushing = false;
    if (isAlreadyFlushing) return;
    isAlreadyFlushing = true;

    std::map<uint32_t, DataNode> snapshot;
    snapshot.swap(records); 
    used_bytes = 0;

    if (saveTheNodesIntoBin(snapshot)) {
        storageManager -> walFrameClearAndSave();
    } else {
        records.merge(snapshot);
        used_bytes = records.size() * 128;
    }
    isAlreadyFlushing = false;
}

bool Buffer::saveTheNodesIntoBin(std::map<uint32_t, DataNode> &snapshot) {
    if (snapshot.empty()) return false;
    if (!transactionRef -> begin()) return false;

    for (auto& [id, data] : snapshot) {
        auto [file_id, offset] = treeRef -> search(id);
        if(file_id != 0xFFFFFFFF && offset != 0xFFFFFFFFFFFFFFFF) {
            storageManager -> overWriteRecord(file_id, offset, data);
        } else {
            auto [rp, file] = storageManager -> getInsertionPosAndFile();
            file -> clear();
            file -> seekp(rp.offset, std::ios::beg);
            file -> write(reinterpret_cast<const char*>(&data), sizeof(data));
            file -> flush();
            treeRef -> insert(id, rp.file_id, rp.offset);
        }
    }

    transactionRef -> commit();
    return true;
}
