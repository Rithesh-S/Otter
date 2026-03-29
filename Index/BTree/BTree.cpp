#include "BTree.h"

// During startup, The tree is needed to be traversed and the mismatch in persisted and active is 
// handled accordingly.
BTree::BTree() : rootPageId(0), nextPageId(1) {
    file.open(indexPath, std::ios::binary | std::ios::in | std::ios::out);
    
    if(!file.is_open()) {
        std::ofstream creator(indexPath, std::ios::binary);
        if(!creator) {
            throw std::runtime_error("\033[31mERROR: Unable to create file:" + indexPath + ".\033[0m");
            return;
        }
        creator.close();
        
        file.open(indexPath, std::ios::binary | std::ios::in | std::ios::out);
    }

    lruCache = std::make_unique<LRUTree>(&file);
    
    file.clear();
    file.seekg(0, std::ios::end);
    if(file.tellp() <= 0) {
        BTNode root;
        root.is_leaf = 1;
        root.n = 0;
        writeNode(0, root);
        nextPageId = 1;
    } else {
        file.seekg(0, std::ios::end);
        nextPageId = static_cast<uint32_t>(file.tellg() / sizeof(BTNode));
    }
}

BTree::~BTree() {
    if(file.is_open()) {
        file.flush();
        file.close();
    }
}

void BTree::writeNode(uint32_t pageId, BTNode& node) {
    file.clear();
    file.seekp(pageId * sizeof(BTNode));
    file.write(reinterpret_cast<const char*>(&node), sizeof(BTNode));
    file.flush();
    lruCache -> updateCache(pageId, node);
}

void BTree::readNode(uint32_t pageId, BTNode& node) {
    lruCache -> getNodeByPageId(pageId, node);
    return;
}

bool BTree::contains(uint32_t key) {
    uint32_t currentPageId = rootPageId;
    while (true) {
        BTNode node;
        readNode(currentPageId, node);
        int i = 0;
        while(i < node.n && key > node.keys[i]) i++;
        if( i < node.n && 
            key == node.keys[i] && 
            node.keyMeta[i].active ) return true;
        if(node.is_leaf) return false;
        currentPageId = node.child_page[i];
    }
}

bool BTree::search(uint32_t key, RecordPointer& rp) {
    uint32_t currentPageId = rootPageId;
    while (true) {
        BTNode node;
        readNode(currentPageId, node);
        int i = 0;
        while(i < node.n && key > node.keys[i]) i++;
        if( i < node.n && 
            key == node.keys[i] && 
            node.keyMeta[i].active
         ) {
                rp = node.keyMeta[i].rp;
                return true; 
        }
        if(node.is_leaf) {
            rp = RecordPointer();
            return false;
        }
        currentPageId = node.child_page[i];
    }
}

bool BTree::markAsDeleted(uint32_t key, RecordPointer& rp) {
    uint32_t currentPageId = rootPageId;
    while (true) {
        BTNode node;
        readNode(currentPageId, node);
        int i = 0;
        while(i < node.n && key > node.keys[i]) i++;
        if(i < node.n && key == node.keys[i]) {
            rp = node.keyMeta[i].rp;
            node.keyMeta[i].active = false;
            writeNode(currentPageId, node);
            return true;
        }
        if(node.is_leaf) {
            rp = RecordPointer();
            return false;
        }
        currentPageId = node.child_page[i];
    }
}

bool BTree::findAndOverWrite(uint32_t key, RecordPointer& rp) {
    uint32_t currentPageId = rootPageId;
    while (true) {
        BTNode node;
        readNode(currentPageId, node);
        int i = 0;
        while(i < node.n && key > node.keys[i]) i++;
        if(i < node.n && key == node.keys[i]) {
            node.keyMeta[i].rp = rp;
            node.keyMeta[i].active = true;
            writeNode(currentPageId, node);
            return true;
        }
        if(node.is_leaf) return false;
        currentPageId = node.child_page[i];
    }
}

bool BTree::insert(uint32_t key, RecordPointer& rp) {
    BTNode root;
    readNode(rootPageId, root); 

    int findIdx = 0;
    while (findIdx < root.n && key > root.keys[findIdx]) findIdx++;
    if (findIdx < root.n && root.keys[findIdx] == key) return false;

    if(root.n == M - 1) {
        BTNode newNode;
        newNode.is_leaf = false;
        newNode.n = 0;

        uint32_t oldRootId = nextPageId++;
        writeNode(oldRootId, root);

        newNode.child_page[0] = oldRootId;

        writeNode(rootPageId, newNode);
        splitChild(rootPageId, 0, oldRootId);
        return insertNonFull(rootPageId, key, rp);
    } else {
        return insertNonFull(rootPageId, key, rp);
    }
    file.flush();
}

void BTree::recovery(uint32_t key, RecordPointer& rp) {
    if(findAndOverWrite(key, rp)) return;
    insert(key, rp);
}

void BTree::splitChild(uint32_t parentID, int i, uint32_t childID) {
    BTNode parent;
    readNode(parentID, parent);
    BTNode child;
    readNode(childID, child);
    BTNode newNode;
    
    int t = M / 2;
    newNode.is_leaf = child.is_leaf;
    newNode.n = t - 1;

    for (int j = 0; j < t - 1; j++) {
        newNode.keys[j] = child.keys[j + t];
        newNode.keyMeta[j] = child.keyMeta[j + t];
    }
    if (!child.is_leaf) {
        for (int j = 0; j < t; j++) {
            newNode.child_page[j] = child.child_page[j + t];
        }
    }

    child.n = t - 1;
    uint32_t newNodeID = nextPageId++;

    for (int j = parent.n; j >= i + 1; j--) parent.child_page[j + 1] = parent.child_page[j];
    parent.child_page[i + 1] = newNodeID;

    for (int j = parent.n - 1; j >= i; j--) {
        parent.keys[j + 1] = parent.keys[j];
        parent.keyMeta[j + 1] = parent.keyMeta[j];
    }
    parent.keys[i] = child.keys[(M / 2) - 1];
    child.n = t - 1; 
    parent.keyMeta[i] = child.keyMeta[(M / 2) - 1];
    parent.n++;

    writeNode(parentID, parent);
    writeNode(childID, child);
    writeNode(newNodeID, newNode);
}

bool BTree::insertNonFull(uint32_t pageId, uint32_t key, RecordPointer& rp) {
    BTNode node;
    readNode(pageId, node);
    int i = node.n - 1;

    int findIdx = 0;
    while (findIdx < node.n && key > node.keys[findIdx]) findIdx++;
    if (findIdx < node.n && node.keys[findIdx] == key) return false;

    if (node.is_leaf) {
        while (i >= 0 && key < node.keys[i]) {
            node.keys[i + 1] = node.keys[i];
            node.keyMeta[i + 1] = node.keyMeta[i];
            i--;
        }
        node.keys[i + 1] = key;
        node.keyMeta[i + 1] = KeyMeta(rp, true);
        node.n++;
        writeNode(pageId, node);
        return true;
    } 
    else {
        i = findIdx;

        BTNode child;
        readNode(node.child_page[i], child);

        if (child.n == M - 1) {
            splitChild(pageId, i, node.child_page[i]);
            readNode(pageId, node); 
            if (key == node.keys[i]) return false; 
            if (key > node.keys[i]) i++;
        }
        return insertNonFull(node.child_page[i], key, rp);
    }
}