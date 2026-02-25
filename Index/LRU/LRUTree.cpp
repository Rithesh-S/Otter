#include "LRUTree.h"

LRUTree::LRUTree(size_t size, std::fstream* file) : size(size), file(file) {}

LRUTree::~LRUTree() {
    lruList.clear();
    lruMap.clear();
}

void LRUTree::getNodeByPageId(uint32_t page_id,BTNode &node) {
    if (lruMap.find(page_id) != lruMap.end()) {
        auto it = lruMap[page_id];
        lruList.splice(lruList.end(), lruList, it);
        node = *(it -> node);
        return;
    }

    if (lruList.size() >= size) {
        uint32_t oldPageId = lruList.front().page_id;
        lruMap.erase(oldPageId);
        lruList.pop_front();
    }

    BTNode newNode;
    file -> clear();
    file -> seekg(page_id * sizeof(BTNode));
    file -> read(reinterpret_cast<char*>(&newNode), sizeof(BTNode));

    lruList.push_back({page_id, std::make_shared<BTNode>(newNode)});
    lruMap[page_id] = std::prev(lruList.end());

    node = *(lruList.back().node);
}

void LRUTree::updateCache(uint32_t page_id, BTNode& node) {
    if (lruMap.find(page_id) != lruMap.end()) {
        auto it = lruMap[page_id];
        *(it -> node) = node;
        lruList.splice(lruList.end(), lruList, it);
        return;
    }

    if (lruList.size() >= size) {
        uint32_t oldPageId = lruList.front().page_id;
        lruMap.erase(oldPageId);
        lruList.pop_front();
    }

    BTNode newNode;
    file -> clear();
    file -> seekg(page_id * sizeof(BTNode));
    file -> read(reinterpret_cast<char*>(&newNode), sizeof(BTNode));

    lruList.push_back({page_id, std::make_shared<BTNode>(newNode)});
    lruMap[page_id] = std::prev(lruList.end());
}