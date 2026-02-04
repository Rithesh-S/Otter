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

std::fstream* LRU::getFileFromLRU(uint32_t file_id) {
    if (lruMap.find(file_id) != lruMap.end()) {
        auto it = lruMap[file_id];
        lruList.splice(lruList.end(), lruList, it);
        return it -> file.get();
    }

    if (lruList.size() >= size) {
        uint32_t oldFile_id = lruList.front().file_id;
        lruList.front().file->close();
        lruMap.erase(oldFile_id);
        lruList.pop_front();
    }

    std::string filePath = sm -> getFilePathByIndex(file_id);
    auto newFile = std::make_unique<std::fstream>(filePath, std::ios::binary | std::ios::in | std::ios::out);

    if(!newFile -> is_open()) {
        std::ofstream creator(filePath, std::ios::binary);
        if(!creator) return nullptr;
        creator.close();

        newFile -> open(filePath, std::ios::binary | std::ios::in | std::ios::out);
    }

    lruList.push_back({file_id, std::move(newFile)});
    lruMap[file_id] = std::prev(lruList.end());

    return lruList.back().file.get();
}