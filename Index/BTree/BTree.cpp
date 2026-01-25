#include "BTree.h"

BTree::BTree(std::string path) : indexPath(path), rootPageId(0), nextPageId(1) {
    file.open(indexPath, std::ios::binary | std::ios::in | std::ios::out);
    
    if(!file.is_open()) {
        std::ofstream creator(indexPath, std::ios::binary);
        creator.close();
        
        file.open(indexPath, std::ios::binary | std::ios::in | std::ios::out);
    }

    file.seekg(0, std::ios::end);
    if(file.tellp() == 0) {
        BTNode root;
        root.is_leaf = 1;
        root.n = 0;
        writeNode(0, root);
        nextPageId = 1;
    } else {
        file.seekg(0, std::ios::end);
        nextPageId = (uint32_t)(file.tellg() / sizeof(BTNode));
    }
}

BTree::~BTree() {
    if(file.is_open()) {
        file.flush();
        file.close();
    }
}

void BTree::writeNode(uint32_t pageId, const BTNode& node) {
    file.seekp(pageId * sizeof(BTNode));
    file.write(reinterpret_cast<const char*>(&node), sizeof(BTNode));
    if(pageId == 0) file.flush();
}

BTNode BTree::readNode(uint32_t pageId) {
    BTNode node;
    file.seekg(pageId * sizeof(BTNode));
    file.read(reinterpret_cast<char*>(&node), sizeof(BTNode));
    return node;
}

RecordPointer BTree::search(uint32_t key) {
    uint32_t currentPageId = rootPageId;
    while (true) {
        BTNode node = readNode(currentPageId);
        int i = 0;
        while(i < node.n && key > node.keys[i]) i++;
        if(i < node.n && key == node.keys[i]) return node.recordPointer[i];
        if(node.is_leaf) return {0xFFFFFFFF, 0};
        currentPageId = node.child_page[i];
    }
}

void BTree::splitChild(uint32_t parentID, int i, uint32_t childID) {
    BTNode parent = readNode(parentID);
    BTNode child = readNode(childID);
    BTNode newNode;
    
    newNode.is_leaf = child.is_leaf;
    newNode.n = (M / 2) - 1;

    for (int j = 0; j < (M / 2) - 1; j++) {
        newNode.keys[j] = child.keys[j + (M / 2)];
        newNode.recordPointer[j] = child.recordPointer[j + (M / 2)];
    }
    if (!child.is_leaf) {
        for (int j = 0; j < (M / 2); j++) {
            newNode.child_page[j] = child.child_page[j + (M / 2)];
        }
    }

    child.n = (M / 2) - 1;
    uint32_t newNodeID = nextPageId++;

    for (int j = parent.n; j >= i + 1; j--) parent.child_page[j + 1] = parent.child_page[j];
    parent.child_page[i + 1] = newNodeID;

    for (int j = parent.n - 1; j >= i; j--) {
        parent.keys[j + 1] = parent.keys[j];
        parent.recordPointer[j + 1] = parent.recordPointer[j];
    }
    parent.keys[i] = child.keys[(M / 2) - 1];
    parent.recordPointer[i] = child.recordPointer[(M / 2) - 1];
    parent.n++;

    writeNode(parentID, parent);
    writeNode(childID, child);
    writeNode(newNodeID, newNode);
}

void BTree::insertNonFull(uint32_t pageId, uint32_t key, uint32_t file_id, uint64_t offset) {
    BTNode node = readNode(pageId);
    int i = node.n - 1;

    if (node.is_leaf) {
        while (i >= 0 && key < node.keys[i]) {
            node.keys[i + 1] = node.keys[i];
            node.recordPointer[i + 1] = node.recordPointer[i];
            i--;
        }
        node.keys[i + 1] = key;
        node.recordPointer[i + 1] = {file_id, offset};
        node.n++;
        writeNode(pageId, node);
    } 
    else {
        while (i >= 0 && key < node.keys[i]) i--;
        i++; 

        BTNode child = readNode(node.child_page[i]);

        if (child.n == M - 1) {
            splitChild(pageId, i, node.child_page[i]);
            node = readNode(pageId); 
            if (key > node.keys[i]) i++;
        }
        insertNonFull(node.child_page[i], key, file_id, offset);
    }
}

void BTree::insert(uint32_t key, uint32_t file_id, uint64_t offset) {
    BTNode root = readNode(rootPageId);
    if(root.n == M - 1) {
        BTNode newNode;
        newNode.is_leaf = false;
        newNode.n = 0;

        uint32_t oldRootId = nextPageId++;
        writeNode(oldRootId, root);

        newNode.child_page[0] = oldRootId;

        writeNode(rootPageId, newNode);
        splitChild(rootPageId, 0, oldRootId);
        insertNonFull(rootPageId, key, file_id, offset);
        if(offset == 0) file.flush();
    } else {
        insertNonFull(rootPageId, key, file_id, offset);
        if(offset == 0) file.flush();
    }
}