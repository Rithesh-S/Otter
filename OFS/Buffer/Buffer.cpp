#include "Buffer.h"
#include "../../StorageManager/StorageManager.h"
#include "../../TransactionManager/Transaction.h"
#include "../../Index/BTree/BTree.h"

Buffer::Buffer(StorageManager *storageManager, BTree *treeRef, Transaction* transactionRef) : 
    storageManager(storageManager), treeRef(treeRef), transactionRef(transactionRef) {
        records.reserve(VEC_SIZE);
    }

std::vector<DataNode> Buffer::readData() { return records; }

void Buffer::removeData(uint32_t id) { 
    auto it = std::find_if(records.begin(), records.end(), [id](const DataNode& node){
        return node.getId() == id;
    });

    if(it != records.end()) {
        *it = std::move(records.back());
        records.pop_back();
    }
}

DataNode Buffer::readData(uint32_t id) { 
    auto it = std::find_if(records.begin(), records.end(), [id](const DataNode& node){
        return node.getId() == id;
    });

    return it != records.end() ? *it : DataNode();
 }

bool Buffer::isFull() { return used_bytes == MAX_BYTES; }

bool Buffer::contains(uint32_t id) { 
    auto it = std::find_if(records.begin(), records.end(), [id](const DataNode& node){
        return node.getId() == id;
    });
    return it != records.end(); 
}
    
void Buffer::writeData(DataNode &record) {
    size_t size = sizeof(DataNode);
    uint32_t id = record.getId();

    if (used_bytes + size > MAX_BYTES) return;
    
    auto it = std::find_if(records.begin(), records.end(), [id](const DataNode& node) {
        return node.getId() == id;
    });
    if(it != records.end()) *it = std::move(record);
    else records.emplace_back(record);

    used_bytes += size;
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
    used_bytes = 0;

    if (saveTheNodesIntoBin(snapshot)) {
        storageManager -> walFrameClearAndSave();
    } else {
        records.insert(records.end(), snapshot.begin(), snapshot.end());
        used_bytes = records.size() * 128;
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
