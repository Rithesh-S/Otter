#include "BufferPool.h"
#include "../../StorageManager/StorageManager.h"

BufferPool::BufferPool(StorageManager* sm) : sm(sm) {}

BufferPool::~BufferPool() { while(!lruList.empty()) evict(); }

bool BufferPool::isFull() { return lruList.size() == POOL_SIZE; }

uint32_t BufferPool::keyCombiner(uint16_t file_id, uint16_t page_no) { return (file_id << 16) | page_no; }

void BufferPool::addFreeSlots(size_t idx) { freeSlots.emplace_back(idx); }

size_t BufferPool::getFreeSlots() {
    if(freeSlots.empty()) return lruList.size();
    size_t idx = freeSlots.front();
    freeSlots.pop_front();
    return idx;
}

Page* BufferPool::getPage(uint16_t file_id, uint16_t page_no) {
    uint32_t combinedKey = keyCombiner(file_id, page_no);
    if (lookUp.find(combinedKey) != lookUp.end()) {
        auto it = lookUp[combinedKey];
        lruList.splice(lruList.end(), lruList, it);
        return frames[*it].getPage();
    }
    
    if(isFull()) evict();

    size_t idx = getFreeSlots();

    Page page;   
    sm -> readPageFromBin(file_id, page_no, page);

    uint32_t newCombinedKey = keyCombiner(file_id, page_no);

    frames[idx] = BufferFrame(file_id, page_no, page);
    frames[idx].makeDirty();

    lruList.emplace_back(idx);
    auto lastElementIt = std::prev(lruList.end());
    lookUp[newCombinedKey] = lastElementIt;

    return frames[idx].getPage();
}

void BufferPool::evict() {
    if(lruList.empty()) return;

    size_t victim_idx = lruList.front();
    auto victim_location = frames[victim_idx].getFileIdAndPageNo();
    uint32_t victim_combined_key = keyCombiner(victim_location.first, victim_location.second);

    if(frames[victim_idx].isDirty()) sm -> writePageIntoBin(frames[victim_idx]);

    lookUp.erase(victim_combined_key);
    lruList.pop_front();
    addFreeSlots(victim_idx);

    return;
}

