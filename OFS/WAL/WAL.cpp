#include "WAL.h"
#include "../../StorageManager/StorageManager.h"
#include "../Buffer/Buffer.h"

std::unique_ptr<WALFrame> WAL::walFrame = nullptr;

WAL::WAL(StorageManager* sm, Buffer* bufferRef, std::string binPath) : storageManager(sm), bufferRef(bufferRef) {
    walFrame = std::make_unique<WALFrame>();
    file.open(binPath, std::ios::binary | std::ios::in | std::ios::out);
    
    if(!file.is_open()) {
        std::ofstream creator(binPath, std::ios::binary);
        if(!creator) return;
        creator.close();
        
        file.open(binPath, std::ios::binary | std::ios::in | std::ios::out);
        saveNodesIntoWALBin();
    }
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

    if(walFrame -> record_count >= 8) {
        storageManager -> writeRecord(readWAL());
        walFrameClear();
        saveNodesIntoWALBin();
        return;
    }
    
    if(walFrame -> record_count != 0) storageManager -> writeRecord(readWAL());
}

void WAL::saveNodesIntoWALBin() { 
    file.seekg(0, std::ios::beg);
    file.write(reinterpret_cast<const char*>(walFrame.get()), sizeof(WALFrame)); 
}

void WAL::walFrameClear() {
    walFrame -> record_count = 0;
    walFrame -> crc = 0;
    for(size_t i = 0; i < 1024; i++) walFrame -> node[i] = '0';
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

    if(walFrame -> record_count >= 8) {
        storageManager -> writeRecord(readWAL());
        walFrameClear();
        saveNodesIntoWALBin();
        return;
    }
}

std::vector<DataNode> WAL::readWAL() {
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(walFrame.get()), sizeof(WALFrame));
    std::vector<DataNode> records;

    for(size_t i = 0; i < walFrame->record_count; i++) {
        char* dest = &walFrame->node[i * nodeSize];
        DataNode* node = reinterpret_cast<DataNode*>(dest);
        auto [id, data] = node->getData();
        // std::cout << "Record ID: " << id << std::endl;
        // std::cout << "Record Data: " << data << std::endl;
        records.push_back(*node);
    }
    return records;
}

bool WAL::verifyCRC() {
    return true;
}


