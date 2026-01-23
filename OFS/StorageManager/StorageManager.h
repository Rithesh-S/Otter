#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "../DataNode/DataNode.h"
#include "../../Index/BTree/BTree.h"

class Buffer;

class StorageManager {

    private:
        static uint32_t index;
        const std::string basepath = "OFS";
        const std::string basepathTree = "Index";
        const std::string metaDataPath = basepath + "/config/metadata.conf";
        const std::string treeIndexPath = basepathTree + "/bin/index.bin";

    public:
        StorageManager();
        ~StorageManager();
        void loadMetaData();
        void saveMetaData();
        void writeRecord(std::ifstream file, uint32_t length, Buffer& buffer);
        std::string readRecord(uint32_t id);
        uint32_t getCurrentBinIndex();
        std::string getFileNameByIndex(uint32_t index);
        std::string getFileNameForBin();
};