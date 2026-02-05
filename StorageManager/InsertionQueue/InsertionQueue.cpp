#include "InsertionQueue.h"
#include "../StorageManager.h"

std::queue<RecordPointer> InsertionQueue::insertionQueue;

InsertionQueue::InsertionQueue(StorageManager* sm, std::string binPath) : storageManager(sm) {
    file.open(binPath, std::ios::binary | std::ios::in | std::ios::out);
    if(!file.is_open()) {
        std::ofstream creator(binPath, std::ios::binary);
        if(!creator) return;
        creator.close();

        file.open(binPath, std::ios::binary | std::ios::in | std::ios::out);
    }
    loadBinData();
}

InsertionQueue::~InsertionQueue() { saveBinData(); }

void InsertionQueue::putRecordPointer(RecordPointer rp) { insertionQueue.push(rp); }

void InsertionQueue::refillQueue(uint32_t file_id) { for(size_t i = 0; i < bufferSize; i += dataNodeSize) putRecordPointer({ file_id, i }); }

RecordPointer InsertionQueue::getRecordPointer() {
    if(insertionQueue.empty()) refillQueue(storageManager -> getNewIndexForBinFlush());
    RecordPointer currRP = insertionQueue.front();
    insertionQueue.pop();
    return currRP;
}

void InsertionQueue::loadBinData() {
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
}

