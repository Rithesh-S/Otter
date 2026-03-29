#include "PersistenceManager.h"

PersistenceManager::PersistenceManager() {
    file.open(metaDataPath, std::ios::binary | std::ios::in | std::ios::out);
    if(!file.is_open()) {
        std::ofstream creator(metaDataPath, std::ios::binary);
        if(!creator) {
            throw std::runtime_error("\033[31mERROR: Unable to create file:" + metaDataPath + ".\033[0m");
            return;
        }
        creator.close();

        file.open(metaDataPath, std::ios::binary | std::ios::in | std::ios::out);
        batchState = { 0, 0 };      // { activeBatch, persistedBatch }
        saveMetaData();
    }
    file.clear();
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(&batchState), sizeof(batchState));

    if (file.gcount() < sizeof(batchState)) {
        batchState = { 0, 0 };
        saveMetaData();
    }
    activeBatch = batchState.first;
    persistedBatch = batchState.second;
}

PersistenceManager::~PersistenceManager() {
    saveMetaData();
    file.close();
}

uint8_t PersistenceManager::getCurrentBatchId() { return activeBatch; }

void PersistenceManager::saveMetaData() {
    batchState = { activeBatch, persistedBatch };
    if (file.is_open()) {
        file.clear();
        file.seekp(0, std::ios::beg);
        
        file.write(reinterpret_cast<const char*>(&batchState), sizeof(batchState));
        file.flush();
    }
}

void PersistenceManager::persistBatch() {
    persistedBatch = activeBatch;
    activeBatch++;
    saveMetaData();
}

bool PersistenceManager::isPersisted(uint8_t keyBatchId) {
    if (keyBatchId == persistedBatch) return true;
    if (activeBatch > persistedBatch) return keyBatchId <= persistedBatch;
    return (keyBatchId <= persistedBatch && keyBatchId > activeBatch);
}


