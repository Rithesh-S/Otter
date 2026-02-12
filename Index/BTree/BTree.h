#pragma once

#include "../data/BTreeNode.h"
#include <fstream>
#include <string>

class StorageManager;

class BTree {
    private:
        std::fstream file;
        uint32_t rootPageId;
        uint32_t nextPageId;
        std::string indexPath;
        StorageManager* storageManager;

        BTNode readNode(uint32_t pageId);
        void writeNode(uint32_t pageId, const BTNode &node);
        void splitChild(uint32_t parentId, int i, uint32_t childId);
        bool findAndOverWrite(uint32_t key, uint32_t file_id, uint64_t offset);
        void insertNonFull(uint32_t pageId, uint32_t key, uint32_t file_id, uint64_t offset);

    public:
        BTree(StorageManager* sm, std::string path);
        ~BTree();

        RecordPointer search(uint32_t key);
        RecordPointer markAsDeleted(uint32_t key);
        void insert(uint32_t key, uint32_t file_id, uint64_t offset);
};
