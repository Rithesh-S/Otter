#pragma once

#include <string>
#include <memory>
#include <fstream>
#include "../LRU/LRUTree.h"
#include "../data/BTreeNode.h"

class BTree {
    private:
        const std::string indexPath = "./Index/bin/index.bin"; 

        std::fstream file;
        uint32_t rootPageId;
        uint32_t nextPageId;
        std::unique_ptr<LRUTree> lruCache;

        void readNode(uint32_t pageId, BTNode& node);
        void writeNode(uint32_t pageId, BTNode &node);
        void splitChild(uint32_t parentId, int i, uint32_t childId);
        void insertNonFull(uint32_t pageId, uint32_t key, RecordPointer& rp);

        bool contains(uint32_t key);

    public:
        BTree();
        ~BTree();
        
        void recovery(uint32_t key, RecordPointer& rp);
        bool persistKey(uint32_t key);

        bool insert(uint32_t key, RecordPointer& rp);
        bool search(uint32_t key, RecordPointer& rp);
        bool markAsDeleted(uint32_t key, RecordPointer& rp);
        bool findAndOverWrite(uint32_t key, RecordPointer& rp);
};
