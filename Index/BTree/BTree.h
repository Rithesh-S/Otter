#pragma once

#include <string>
#include <memory>
#include <fstream>
#include "../LRU/LRUTree.h"
#include "../data/BTreeNode.h"

class StorageManager;

class BTree {
    private:
        static const uint32_t cacheSize;

        std::fstream file;
        uint32_t rootPageId;
        uint32_t nextPageId;
        std::string indexPath;
        StorageManager* storageManager;
        std::unique_ptr<LRUTree> lruCache;

        BTNode readNode(uint32_t pageId);
        void writeNode(uint32_t pageId, BTNode &node);
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
