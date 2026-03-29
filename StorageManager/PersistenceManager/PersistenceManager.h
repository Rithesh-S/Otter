#pragma once

#include <string>
#include <cstdint>
#include <fstream>

class PersistenceManager {
    private: 
        const std::string metaDataPath = "./StorageManager/PersistenceManager/config/metadata.conf";

        std::fstream file;
        uint8_t activeBatch;
        uint8_t persistedBatch;
        std::pair<uint8_t, uint8_t> batchState;

        void saveMetaData();

    public:
        PersistenceManager();
        ~PersistenceManager();

        void persistBatch();
        bool isPersisted(uint8_t keyBatchId);
        uint8_t getCurrentBatchId();

};