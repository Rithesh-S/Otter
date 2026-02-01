#include "WAL.h"
#include "../../StorageManager/StorageManager.h"
#include "../Buffer/Buffer.h"

std::unique_ptr<WALFrame> WAL::walFrame = nullptr;

WAL::WAL(StorageManager* sm, Buffer* bufferRef) : storageManager(sm), bufferRef(bufferRef) {
    walFrame = std::make_unique<WALFrame>();
    file.open(walBinPath, std::ios::binary | std::ios::in | std::ios::out);

    if(!file.is_open()) {
        std::ofstream creator(walBinPath, std::ios::binary);
        creator.close();

        file.open(walBinPath, std::ios::binary | std::ios::in | std::ios::out);
        saveNodesIntoWALBin();
    }
    loadWALData();
}

WAL::~WAL() {
    if(file.is_open()) {
        file.flush();
        file.close();
    }
}

void WAL::loadWALData() {
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(walFrame.get()), sizeof(WALFrame));
    if(walFrame -> record_count != 0) {
        std::vector<DataNode> records = readWAL();
        for(auto node: records) {
            auto [id, data] = node.getData();
            bufferRef->writeData(id, node, sizeof(node));
        }
    }
}

void WAL::saveNodesIntoWALBin() { 
    file.seekg(0, std::ios::beg);
    file.write(reinterpret_cast<const char*>(walFrame.get()), sizeof(WALFrame)); 
}

void WAL::writeWAL(DataNode& node) {
    if(walFrame -> magic != magic && !verifyCRC())  {
        std::cerr << "\033[31mERROR:Inappropiate WAL File detected.\033[0m" << std::endl;
        return;
    }
    
    char* dest = &walFrame->node[walFrame->record_count * nodeSize];
    std::memcpy(dest, &node, nodeSize);
    walFrame -> record_count++;
    saveNodesIntoWALBin();
    readWAL();
}

std::vector<DataNode> WAL::readWAL() {
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(walFrame.get()), sizeof(WALFrame));
    std::vector<DataNode> records;

    for(size_t i = 0; i < walFrame->record_count; i++) {
        char* dest = &walFrame->node[i * nodeSize];
        DataNode* node = reinterpret_cast<DataNode*>(dest);
        auto [id, data] = node->getData();
        std::cout << "Record ID: " << id << std::endl;
        std::cout << "Record Data: " << data << std::endl;
        records.push_back(*node);
    }
    return records;
}

bool WAL::verifyCRC() {
    return true;
}


