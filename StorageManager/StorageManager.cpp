#include "StorageManager.h"
#include "../OFS/Buffer/Buffer.h"

uint32_t StorageManager::index = 0xFFFFFFFF;
std::unique_ptr<BTree> StorageManager::tree = nullptr;
std::unique_ptr<Buffer> StorageManager::buffer = nullptr;
std::unique_ptr<WAL> StorageManager::wal = nullptr;

StorageManager::~StorageManager() { saveMetaData(); }

StorageManager::StorageManager() { 
    tree = std::make_unique<BTree>(this, treeIndexPath);
    buffer = std::make_unique<Buffer>(this, StorageManager::tree.get());
    wal = std::make_unique<WAL>(this, StorageManager::buffer.get());
    loadMetaData(); 
}

void StorageManager::loadMetaData() {
    std::ifstream inFile(metaDataPath);
    if(inFile.is_open()) {
        inFile >> index;
        inFile.close();
    } else {
        index = 0xFFFFFFFF;
        saveMetaData();
    }
}

std::string StorageManager::getFileNameForBin() {
    index++;
    saveMetaData();
    std::string binFileName = basepath + "/Buffer/bin/chunk_file_" + std::to_string(index) + ".bin";
    return binFileName;
}

std::string StorageManager::readRecord(uint32_t id) {

    if(buffer -> contains(id)) {
       auto data = buffer -> readData(id).getData();
       return std::to_string(data.first) + " - " + data.second;
    }

    auto [file_id, offset] = tree -> search(id);
    std::string fileName = getFileNameByIndex(file_id);

    std::ifstream inFile(fileName, std::ios::binary);
    DataNode dataNode;

    if (!inFile) return "No Records Found";

    inFile.seekg(offset);
    
    if(inFile.read(reinterpret_cast<char*>(&dataNode), sizeof(DataNode))) {
        auto data = dataNode.getData();
        return std::to_string(data.first) + " - " + data.second;
    }
    return "No Records Found";
}

void StorageManager::writeRecord(std::ifstream& file) {
    std::string line;
    while (std::getline(file, line)) {
        uint32_t id;
        std::string temp_id, msg;
        char buf[length] = {0};

        std::stringstream ss(line);
        
        if (std::getline(ss, temp_id, ',')) id = std::stoull(temp_id);
        std::getline(ss, msg, ',');
        
        if (msg.size() > length) std::cerr << "\033[33mWARNING: The data size exceeds " << length << ", hence excess length is truncated.\033[0m" << std::endl;
        
        std::strncpy(buf, msg.c_str(), length);
        DataNode dataNode = DataNode(id, buf);
        wal -> writeWAL(dataNode);

        if(buffer -> contains(id) || (tree->search(id).file_id != 0xFFFFFFFF)) {
            std::cerr << "\033[33mWARNING: Duplicate ID found, Hence Ignored.\033[0m" << std::endl;
            continue;
        }

        buffer -> writeData(id, dataNode, sizeof(dataNode));

        if (buffer -> isFull()) buffer -> flush();
    }
}

void StorageManager::writeRecord(uint32_t id, std::string msg) { 
    char buf[length] = {0};
    
    if (msg.size() > length) std::cerr << "\033[33mWARNING: The data size exceeds " << length << ", hence excess length is truncated.\033[0m" << std::endl;
    
    std::strncpy(buf, msg.c_str(), length);
    DataNode dataNode = DataNode(id, buf);
    wal -> writeWAL(dataNode);

    if(buffer -> contains(id) || (tree->search(id).file_id != 0xFFFFFFFF)) {
        std::cerr << "\033[33mWARNING: Duplicate ID found, Hence Ignored.\033[0m" << std::endl;
        return;
    }
    buffer -> writeData(id, dataNode, sizeof(dataNode));
    if (buffer -> isFull()) buffer -> flush();
}

void StorageManager::writeRecord(std::vector<DataNode> walBuf) { 
    for(auto& node: walBuf) {
        uint32_t id = node.getData().first;
        buffer -> writeData(id, node, sizeof(node));
        if (buffer -> isFull()) buffer -> flush();
    }
}

std::string StorageManager::getFileNameByIndex(uint32_t index) {
    std::string binFileName = basepath + "/Buffer/bin/chunk_file_" + std::to_string(index) + ".bin";
    return binFileName;
}

std::string StorageManager::getBTreeIndexPath() { return treeIndexPath; }

uint32_t StorageManager::getCurrentBinIndex() { return index; }

void StorageManager::saveMetaData() {
    std::ofstream outFile(metaDataPath, std::ios::trunc);
    if(outFile.is_open()) {
        outFile << index;
        outFile.flush();
        outFile.close();
    }
}

