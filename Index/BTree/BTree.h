#pragma once

#include "../data/BTreeNode.h"
#include <fstream>
#include <string>

class StorageManager;

class BTree {
    private:
        std::string indexPath;
        uint32_t rootPageId;
        uint32_t nextPageId;
        std::fstream file;
        StorageManager* storageManager;

        void writeNode(uint32_t pageId, const BTNode &node);
        BTNode readNode(uint32_t pageId);
        void splitChild(uint32_t parentId, int i, uint32_t childId);
        void insertNonFull(uint32_t pageId, uint32_t key, uint32_t file_id, uint64_t offset);

    public:
        BTree(StorageManager* sm, std::string path);
        ~BTree();
        void insert(uint32_t key, uint32_t file_id, uint64_t offset);
        RecordPointer search(uint32_t key);
};
