#pragma once

#include <list>
#include <memory>
#include <fstream>
#include <unordered_map>
#include "../data/BTreeNode.h"
#include "./data/NodePointer.h"

class LRUTree {
    private:
        size_t size;
        std::list<CachedNode> lruList;
        std::unordered_map<uint32_t, std::list<CachedNode>::iterator> lruMap;

        std::fstream* file;

    public:
        LRUTree(size_t size, std::fstream* file);
        ~LRUTree();

        void getNodeByPageId(uint32_t page_id, BTNode &node);
        void updateCache(uint32_t page_id, BTNode &node);
};