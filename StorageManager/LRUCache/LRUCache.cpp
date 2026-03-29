#include "LRUCache.h"

LRU::LRU() { loadMetaData(); }

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

    saveMetaData();
    metaFile.close();
}

std::pair<uint16_t, uint16_t> LRU::getNewIndex() {
    std::pair<uint16_t, uint16_t> newIndexPair = indexPair;

    if(indexPair.second == 2047) indexPair = { indexPair.first + 1, 0 };
    else indexPair.second++;
    saveMetaData();
    
    return newIndexPair;
}

void LRU::loadMetaData() {
    metaFile.open(metaDataPath, std::ios::binary | std::ios::in | std::ios::out);
    if(!metaFile.is_open()) {
        std::ofstream creator(metaDataPath, std::ios::binary);
        if(!creator) {
            throw std::runtime_error("\033[31mERROR: Unable to create file:" + metaDataPath + ".\033[0m");
            return;
        }
        creator.close();

        metaFile.open(metaDataPath, std::ios::binary | std::ios::in | std::ios::out);
        indexPair = { 0, 0 };
        saveMetaData();
    }
    metaFile.clear();
    metaFile.seekg(0, std::ios::beg);
    metaFile.read(reinterpret_cast<char*>(&indexPair), sizeof(indexPair));

    if (metaFile.gcount() < sizeof(indexPair)) {
        indexPair = { 0, 0 };
        saveMetaData();
    }
}

void LRU::saveMetaData() {
    if (metaFile.is_open()) {
        metaFile.clear();
        metaFile.seekp(0, std::ios::beg);
        
        metaFile.write(reinterpret_cast<const char*>(&indexPair), sizeof(indexPair));
        metaFile.flush();
    }
}

std::string LRU::getFilePathByIndex(uint32_t index) {
    std::string binFileName = "OFS/bin/chunk_file_" + std::to_string(index) + ".bin";
    return binFileName;
}

std::fstream* LRU::getFileFromLRU(uint32_t file_id) {
    if (lruMap.find(file_id) != lruMap.end()) {
        auto it = lruMap[file_id];
        lruList.splice(lruList.end(), lruList, it);
        return it -> file.get();
    }

    if (lruList.size() >= CACHE_SIZE) {
        uint32_t oldFile_id = lruList.front().file_id;
        lruList.front().file->close();
        lruMap.erase(oldFile_id);
        lruList.pop_front();
    }

    std::string filePath = getFilePathByIndex(file_id);
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
    std::fstream* finalPtr = lastElementIt -> file.get();
    
    if (!finalPtr) throw std::runtime_error("LRU Critical: Unique_ptr delivered a nullptr.");
    return finalPtr;
}