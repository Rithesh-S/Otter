#include "Buffer.h"
#include "../../StorageManager/StorageManager.h"
#include "../../TransactionManager/Transaction.h"
#include "../../Index/BTree/BTree.h"

Buffer::Buffer(StorageManager *storageManager, BTree *treeRef, Transaction* transactionRef) : 
    storageManager(storageManager), treeRef(treeRef), transactionRef(transactionRef) {}

std::map<uint32_t, DataNode> Buffer::readData() { return records; }

void Buffer::removeData(uint32_t id) { records.erase(id); }

DataNode Buffer::readData(uint32_t id) { return records[id]; }

bool Buffer::isFull() { return used_bytes == max_bytes; }

bool Buffer::contains(uint32_t id) { return records.find(id) != records.end(); }
    
void Buffer::writeData(uint32_t id, DataNode &record, size_t size) {
    if (used_bytes + size > max_bytes) return;
    records[id] = record;
    used_bytes += size;
}

void Buffer::flush() {
    if (saveTheNodesIntoBin(records)) {
        records.clear();
        storageManager -> walFrameClearAndSave();
        used_bytes = 0;
    }
    else return;
}

bool Buffer::saveTheNodesIntoBin(std::map<uint32_t, DataNode> &records) {
    if (records.empty()) return false;
    if (!transactionRef -> begin()) return false;

    for (auto [id, data] : records) {
        auto [file_id, offset] = treeRef -> search(id);
        if(file_id != 0xFFFFFFFF && offset != 0xFFFFFFFFFFFFFFFF) storageManager -> overWriteRecord(file_id, offset, data);
        else {
            auto [rp, file] = storageManager -> getInsertionPosAndFile();
            file -> seekg(rp.offset, std::ios::beg);
            file -> write(reinterpret_cast<const char*>(&data), sizeof(data));
            file -> flush();
            treeRef -> insert(id, rp.file_id, rp.offset);
        }
    }

    transactionRef -> commit();
    return true;
}
