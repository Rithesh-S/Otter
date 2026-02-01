#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <memory>
#include <iostream>
#include <filesystem>
#include <vector>
#include "../Index/BTree/BTree.h"
#include "../OFS/DataNode/DataNode.h"
#include "../OFS/WAL/WAL.h"

class Buffer;

class StorageManager {

    private:
        static uint32_t index;
        static const uint8_t length = 124;
        const std::string basepath = "OFS";
        const std::string basepathTree = "Index";
        const std::string metaDataPath = basepath + "/Buffer/config/metadata.conf";
        const std::string treeIndexPath = basepathTree + "/bin/index.bin";
        static std::unique_ptr<BTree> tree;
        static std::unique_ptr<Buffer> buffer;
        static std::unique_ptr<WAL> wal;

        void writeRecord(std::vector<DataNode> walBuf);

    public:
        StorageManager();
        ~StorageManager();
        
        void loadMetaData();
        void saveMetaData();
        void writeRecord(std::ifstream& file);
        void writeRecord(uint32_t id, std::string msg);
        std::string readRecord(uint32_t id);
        uint32_t getCurrentBinIndex();
        std::string getFileNameByIndex(uint32_t index);
        std::string getBTreeIndexPath();
        std::string getFileNameForBin();

        // friend class WAL;
};