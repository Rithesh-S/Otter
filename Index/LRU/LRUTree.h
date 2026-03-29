#pragma once

#include <list>
#include <memory>
#include <fstream>
#include <unordered_map>
#include "../data/BTreeNode.h"
#include "./data/NodePointer.h"

class LRUTree {
    private:
        static const size_t CACHE_SIZE = 256;

        BTNode frames[CACHE_SIZE];
        uint32_t pageMapping[CACHE_SIZE];
        std::list<size_t> lruList;
        std::list<size_t> freeSlots;
        std::unordered_map<uint32_t, std::list<size_t>::iterator> lookUp;

        std::fstream* file;

        size_t getFreeSlot();
        bool isFull();

    public:
        LRUTree(std::fstream* file);

        void getNodeByPageId(uint32_t page_id, BTNode& node);
        void updateCache(uint32_t page_id, BTNode &node);
};