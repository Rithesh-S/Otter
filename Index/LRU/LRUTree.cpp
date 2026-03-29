#include "LRUTree.h"

LRUTree::LRUTree(std::fstream* file) : file(file) {
    for (size_t i = 0; i < CACHE_SIZE; i++) freeSlots.push_back(i);
}

bool LRUTree::isFull() { 
    return lruList.size() >= CACHE_SIZE; 
}

size_t LRUTree::getFreeSlot() {
    if (freeSlots.empty()) return 0;
    size_t idx = freeSlots.front();
    freeSlots.pop_front();
    return idx;
}

void LRUTree::getNodeByPageId(uint32_t page_id, BTNode& node) {
    auto it = lookUp.find(page_id);
    
    if (it != lookUp.end()) {
        lruList.splice(lruList.end(), lruList, it -> second);
        node = frames[*(it -> second)]; 
        return;
    }

    if (isFull()) {
        size_t victimIdx = lruList.front();
        uint32_t victimPageId = pageMapping[victimIdx];
        
        lookUp.erase(victimPageId);
        lruList.pop_front();
        freeSlots.push_back(victimIdx);
    }

    size_t idx = getFreeSlot();

    file -> clear();
    file -> seekg((std::streamoff)page_id * sizeof(BTNode));
    file -> read(reinterpret_cast<char*>(&frames[idx]), sizeof(BTNode));

    pageMapping[idx] = page_id;
    lruList.push_back(idx);
    lookUp[page_id] = std::prev(lruList.end());

    node = frames[idx];
}

void LRUTree::updateCache(uint32_t page_id, BTNode& node) {
    auto it = lookUp.find(page_id);
    
    if (it != lookUp.end()) {
        size_t idx = *(it -> second);
        frames[idx] = node;
        lruList.splice(lruList.end(), lruList, it -> second);
        return;
    }

    if (isFull()) {
        size_t victimIdx = lruList.front();
        uint32_t victimPageId = pageMapping[victimIdx];
        
        lookUp.erase(victimPageId);
        lruList.pop_front();
        freeSlots.push_back(victimIdx);
    }

    size_t idx = getFreeSlot();
    
    frames[idx] = node;
    pageMapping[idx] = page_id;
    lruList.push_back(idx);
    lookUp[page_id] = std::prev(lruList.end());
}