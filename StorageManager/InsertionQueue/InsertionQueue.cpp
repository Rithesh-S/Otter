#include "InsertionQueue.h"
#include "../StorageManager.h"

std::queue<RecordPointer> InsertionQueue::insertionQueue;

InsertionQueue::InsertionQueue(StorageManager* sm) : storageManager(sm) {
    file.open(binPath, std::ios::binary | std::ios::in | std::ios::out);
    if(!file.is_open()) {
        std::ofstream creator(binPath, std::ios::binary);
        if(!creator) {
            throw std::runtime_error("\033[31mERROR: Unable to create file:" + binPath + ".\033[0m");
            exit(1);
        }
        creator.close();

        file.open(binPath, std::ios::binary | std::ios::in | std::ios::out);
    }
    loadBinData();
}

InsertionQueue::~InsertionQueue() { saveBinData(); }

void InsertionQueue::putRecordPointer(RecordPointer rp) { insertionQueue.push(rp); }

void InsertionQueue::refillQueue(std::pair<uint16_t, uint16_t> indexPair) { 
    for(size_t i = 0; i < RECORD_COUNT; ++i) 
        putRecordPointer({ indexPair.first, indexPair.second }); 
}

RecordPointer InsertionQueue::getRecordPointer() {
    if(insertionQueue.empty()) refillQueue(storageManager -> getNewIndexForBinFlush());
    RecordPointer currRP = insertionQueue.front();
    insertionQueue.pop();
    return currRP;
}

void InsertionQueue::loadBinData() {
    file.clear();
    file.seekg(0, std::ios::beg);
    RecordPointer temp;
    while(file.read(reinterpret_cast<char*>(&temp), sizeof(RecordPointer))) { insertionQueue.push(temp); }
}

void InsertionQueue::saveBinData() {
    file.clear();
    file.seekp(0, std::ios::beg);
    while(!insertionQueue.empty()) {
        file.write(reinterpret_cast<const char*>(&insertionQueue.front()), sizeof(RecordPointer));
        insertionQueue.pop();
    }
    file.flush();
    file.close();
}

