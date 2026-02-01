#include "WAL.h"
#include "../../StorageManager/StorageManager.h"

std::unique_ptr<WALFrame> WAL::walFrame = nullptr;

WAL::WAL(StorageManager* sm) : storageManager(sm) {
    walFrame = std::make_unique<WALFrame>();
    file.open(walBinPath, std::ios::binary | std::ios::in | std::ios::out);

    if(!file.is_open()) {
        std::ofstream creator(walBinPath, std::ios::binary);
        creator.close();

        file.open(walBinPath, std::ios::binary | std::ios::in | std::ios::out);
        saveNodesIntoWALBin();
    }
    file.seekg(0, std::ios::end);

    if(file.tellp() != 0) { 
        // loadWALData();
    }
    // readWAL();
}

WAL::~WAL() {
    if(file.is_open()) {
        file.flush();
        file.close();
    }
}

void WAL::loadWALData() {
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(walFrame.get()), sizeof(walFrame));
    if(walFrame -> record_count != 0) {

    }
}

void WAL::saveNodesIntoWALBin() { 
    file.seekg(0, std::ios::beg);
    file.write(reinterpret_cast<const char*>(walFrame.get()), sizeof(walFrame)); 
}

void WAL::writeWAL(DataNode& node) {
    if(walFrame -> magic != magic && !verifyCRC())  {
        std::cerr << "\033[31mERROR:Inappropiate WAL File detected.\033[0m" << std::endl;
        return;
    }
    
    char* dest = &walFrame->node[walFrame->record_count * nodeSize];
    walFrame -> record_count++;
    std::memcpy(dest, &node, nodeSize);
    saveNodesIntoWALBin();
    readWAL();
}

void WAL::readWAL() {
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(walFrame.get()), sizeof(walFrame));
    std::cout << "--- WAL Frame Debug ---" << std::endl;
    std::cout << "Record Count: " << (int)walFrame->record_count << std::endl;

    for(size_t i = 0; i < walFrame->record_count; i++) {
        char* dest = &walFrame->node[i * nodeSize];
        DataNode* node = reinterpret_cast<DataNode*>(dest);
        auto [id, data] = node->getData();
        std::cout << "Record ID: " << id << std::endl;
        std::cout << "Record Data: " << data << std::endl;
    }
}

bool WAL::verifyCRC() {
    return true;
}


