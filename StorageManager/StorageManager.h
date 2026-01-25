#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <memory>
#include <iostream>
#include <filesystem>
#include "../Index/BTree/BTree.h"
#include "../OFS/DataNode/DataNode.h"

class Buffer;

class StorageManager {

    private:
        static uint32_t index;
        static const uint32_t length = 124;
        const std::string basepath = "OFS";
        const std::string basepathTree = "Index";
        const std::string metaDataPath = basepath + "/config/metadata.conf";
        const std::string treeIndexPath = basepathTree + "/bin/index.bin";
        static std::unique_ptr<BTree> tree;

    public:
        StorageManager();
        ~StorageManager();
        void loadMetaData();
        void saveMetaData();
        void writeRecord(std::ifstream& file, Buffer& buffer);
        void writeRecord(uint32_t id, std::string msg, Buffer& buffer);
        std::string readRecord(uint32_t id, Buffer& buffer);
        uint32_t getCurrentBinIndex();
        std::string getFileNameByIndex(uint32_t index);
        std::string getFileNameForBin();
};