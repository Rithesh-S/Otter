#include "StorageManager.h"
#include "../Buffer/Buffer.h"

size_t StorageManager::index = -1;
StorageManager::StorageManager() { loadMetaData(); }
StorageManager::~StorageManager() { saveMetaData(); }

void StorageManager::loadMetaData() {
    std::ifstream inFile(metaDataPath);

    if(inFile.is_open()) {
        inFile >> index;
        inFile.close();
        // std::cout << "\033[32mSUCCESS: DB Engine resumed at bin id: " << index << ".\033[0m" << std::endl;   
    } else {
        index = -1;
        saveMetaData();
    }
}

std::string StorageManager::getFileNameForBin() {
    index++;
    saveMetaData();
    std::string binFileName = basepath + "/bin/chunk_file_" + std::to_string(index) + ".bin";
    return binFileName;
}

std::string StorageManager::readRecord(size_t offset ,std::string fileName) {
    std::ifstream inFile(fileName, std::ios::binary );
    DataNode dataNode;

    if (!inFile) return nullptr;

    size_t pos = offset * sizeof(DataNode);
    inFile.seekg(pos);
    
    if(inFile.read(reinterpret_cast<char*>(&dataNode), sizeof(DataNode))) {
        auto data = dataNode.getData();
        return std::to_string(data.first) + " - " + data.second;
    }
    return nullptr;
}

void StorageManager::writeRecord(std::ifstream file, uint32_t length, Buffer& buffer) {
    std::string line;

    while (std::getline(file, line)) {
        uint32_t id;
        std::string temp_id, msg;
        char buf[124] = {0};

        std::stringstream ss(line);
        
        if (std::getline(ss, temp_id, ','))
        id = std::stoull(temp_id);
        std::getline(ss, msg, ',');
        
        if (msg.size() > 124)
        std::cerr << "\033[33mWARNING: The data size exceeds 124, hence excess length is truncated.\033[0m" << std::endl;
        
        std::strncpy(buf, msg.c_str(), length);
        
        DataNode dataNode = DataNode(id, buf);

        buffer.writeData(id, dataNode, sizeof(dataNode));

        if (buffer.isFull()) buffer.flush();
    }
}

std::string StorageManager::getFileNameByIndex(size_t index) {
    std::string binFileName = basepath + "/bin/chunk_file_" + std::to_string(index) + ".bin";
    return binFileName;
}

void StorageManager::saveMetaData() {
    std::ofstream outFile(metaDataPath, std::ios::trunc);
    if(outFile.is_open()) {
        outFile << index;
        outFile.close();
        // std::cout << "\033[32mSUCCESS: DB Engine stored last used bin id: " << index << ".\033[0m" << std::endl;   
    }
}

