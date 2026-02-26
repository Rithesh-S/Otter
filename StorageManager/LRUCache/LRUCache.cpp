#include "LRUCache.h"
#include "../StorageManager.h"

LRU::LRU(StorageManager *sm, size_t size) : sm(sm), size(size) {}

LRU::~LRU() {
    for(auto& entry: lruList) {
        entry.file -> clear();
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
    std::unique_ptr<std::fstream> newFile = std::make_unique<std::fstream>();
    newFile -> open(filePath, std::ios::binary | std::ios::in | std::ios::out);
    
    int attempt = 5;
    while(!newFile -> is_open() && (attempt-- > 0)) {
        {
            std::ofstream creator(filePath, std::ios::binary);
            if(!creator) {
                throw std::runtime_error("\033[31mERROR:Unable to create file:" + filePath + ".\033[0m");
                return nullptr;
            }
            creator.flush();
            creator.close();
        }

        newFile -> clear();
        newFile -> open(filePath, std::ios::binary | std::ios::in | std::ios::out);
    }
    
    lruList.emplace_back(file_id, std::move(newFile));
    auto lastElementIt = std::prev(lruList.end());
    
    lruMap[file_id] = lastElementIt;
    std::fstream* finalPtr = lastElementIt->file.get();
    
    if (!finalPtr) throw std::runtime_error("LRU Critical: Unique_ptr delivered a nullptr.");
    return finalPtr;
}