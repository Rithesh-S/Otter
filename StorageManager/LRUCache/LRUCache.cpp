#include "LRUCache.h"
#include "../StorageManager.h"

LRU::LRU(StorageManager *sm, size_t size) : sm(sm), size(size) {}

LRU::~LRU() {
    for(auto& entry: lruList) {
        if(entry.file && entry.file -> is_open()) {
            entry.file -> flush();
            entry.file -> close();
        }
    }
    lruList.clear();
    lruMap.clear();
}

std::unique_ptr<std::fstream> LRU::getFileFromLRU(uint32_t file_id) {
    if (lruMap.find(file_id) != lruMap.end()) {
        auto it = lruMap[file_id];
        lruList.splice(lruList.end(), lruList, it);
        return std::move(it -> file);
    }

    if (lruList.size() >= size) {
        uint32_t oldFile_id = lruList.front().file_id;
        lruList.front().file->close();
        lruMap.erase(oldFile_id);
        lruList.pop_front();
    }

    auto newfile = std::make_unique<std::fstream>(sm -> getFilePathByIndex(file_id), std::ios::binary | std::ios::in | std::ios::out);
    if(!newfile->is_open()) return nullptr;
    lruList.push_back({file_id, std::move(newfile)});
    lruMap[file_id] = std::prev(lruList.end());

    return std::move(lruList.back().file);
}