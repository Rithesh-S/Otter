#include "Buffer.h"
#include "../../StorageManager/StorageManager.h"
#include "../../TransactionManager/Transaction.h"
#include "../../Index/BTree/BTree.h"

Buffer::Buffer(StorageManager *storageManager, BTree *treeRef, Transaction* transactionRef) : 
    storageManager(storageManager), treeRef(treeRef), transactionRef(transactionRef) {
        records.reserve(VEC_SIZE);
    }
    
std::vector<DataNode> Buffer::readData() { return records; }

bool Buffer::isFull() { return used_bytes == MAX_BYTES; }

bool Buffer::contains(uint32_t id) { return indexMap.find(id) != indexMap.end(); }

void Buffer::removeData(uint32_t id) { 
    auto it = indexMap.find(id);
    if(it == indexMap.end()) return;

    size_t indexToBeDeleted = it -> second;
    
    if (indexToBeDeleted < records.size() - 1) {
        uint32_t lastElementId = records.back().getId();
        records[indexToBeDeleted] = std::move(records.back());
        indexMap[lastElementId] = indexToBeDeleted;
    }
    
    records.pop_back();
    indexMap.erase(it);
}

DataNode Buffer::readData(uint32_t id) { 
    auto it = indexMap.find(id);
    if(it != indexMap.end()) return records[it->second];
    return DataNode();
}
    
void Buffer::writeData(DataNode &record) {
    size_t size = sizeof(DataNode);
    uint32_t id = record.getId();

    if(contains(id)) {
        records[indexMap[id]] = std::move(record);
    } else {
        if (used_bytes + size > MAX_BYTES) return;
        indexMap[id] = records.size();
        records.emplace_back(std::move(record));
        used_bytes += size;
    }
}

void Buffer::writeRecordsFromWal(std::vector<DataNode> walBuf) {
    for (auto &node : walBuf) {
        uint32_t id = node.getId();
        if(node.isEmpty()) removeData(id);
        else writeData(node);
    }
}

void Buffer::flush() {
    static bool isAlreadyFlushing = false;
    if (isAlreadyFlushing) return;
    isAlreadyFlushing = true;

    std::vector<DataNode> snapshot;
    snapshot.swap(records); 
    indexMap.clear();
    used_bytes = 0;

    if (saveTheNodesIntoBin(snapshot)) {
        storageManager -> walFrameClearAndSave();
    } else {
        for(size_t i = 0; i < snapshot.size(); ++i) indexMap[snapshot[i].getId()] = i;
        records = std::move(snapshot);
        used_bytes = records.size() * sizeof(DataNode);
    }
    isAlreadyFlushing = false;
}

bool Buffer::saveTheNodesIntoBin(std::vector<DataNode> &snapshot) {
    if (snapshot.empty()) return false;
    if (!transactionRef -> begin()) return false;

    std::unordered_set<std::fstream*> touchedFiles;

    for (auto& it : snapshot) {
        uint32_t id = it.getId();
        auto [file_id, offset] = treeRef -> search(id);

        if(file_id != 0xFFFFFFFF && offset != 0xFFFFFFFFFFFFFFFF) {
            storageManager -> overWriteRecord(file_id, offset, it);
        } else {
            auto [rp, file] = storageManager -> getInsertionPosAndFile();

            file -> clear();
            file -> seekp(rp.offset, std::ios::beg);

            file -> write(reinterpret_cast<const char*>(&it), sizeof(it));
            touchedFiles.insert(file);

            treeRef -> insert(id, rp.file_id, rp.offset);
        }
    }

    for (auto* f : touchedFiles) f -> flush();

    transactionRef -> commit();
    return true;
}
