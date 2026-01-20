#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "../DataNode/DataNode.h"

class Buffer;

class StorageManager {

    private:
        static size_t index;
        const std::string basepath = "OFS";
        const std::string metaDataPath = basepath + "/config/metadata.conf";

    public:
        StorageManager();
        ~StorageManager();
        void loadMetaData();
        void saveMetaData();
        void writeRecord(std::ifstream file, uint32_t length, Buffer& buffer);
        std::string readRecord(size_t offset, std::string filePath);
        std::string getFileNameByIndex(size_t index);
        std::string getFileNameForBin();
};