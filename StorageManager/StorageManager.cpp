#include "StorageManager.h"
#include "../OFS/Buffer/Buffer.h"

uint32_t StorageManager::index = 0xFFFFFFFF;
const size_t StorageManager::cacheSize = 50;
const uint8_t StorageManager::length = 124;

std::unique_ptr<BTree> StorageManager::tree = nullptr;
std::unique_ptr<Buffer> StorageManager::buffer = nullptr;
std::unique_ptr<WAL> StorageManager::wal = nullptr;
std::unique_ptr<LRU> StorageManager::lruCache = nullptr;

StorageManager::~StorageManager() { saveMetaData(); }

StorageManager::StorageManager() { 
    tree = std::make_unique<BTree>(this, treeIndexPath);
    buffer = std::make_unique<Buffer>(this, StorageManager::tree.get());
    wal = std::make_unique<WAL>(this, StorageManager::buffer.get());
    lruCache = std::make_unique<LRU>(this, cacheSize);
    loadMetaData();
}

std::string StorageManager::getBTreeIndexPath() { return treeIndexPath; }

std::string StorageManager::getWALBinPath() { return walBinPath; }

uint32_t StorageManager::getCurrentBinIndex() { return index; }

std::string StorageManager::getFilePathByIndex(uint32_t index) {
    std::string binFileName = basepath + "/Buffer/bin/chunk_file_" + std::to_string(index) + ".bin";
    return binFileName;
}

std::unique_ptr<std::fstream> StorageManager::getFileByIndex(uint32_t index) { return lruCache -> getFileFromLRU(index); }

std::string StorageManager::getFilePathForBinFlush() {
    index++;
    saveMetaData();
    std::string binFileName = basepath + "/Buffer/bin/chunk_file_" + std::to_string(index) + ".bin";
    return binFileName;
}

void StorageManager::saveMetaData() {
    std::ofstream outFile(metaDataPath, std::ios::trunc);
    if (outFile.is_open()) {
        outFile << index;
        outFile.flush();
        outFile.close();
    }
}

void StorageManager::loadMetaData() {
    std::ifstream inFile(metaDataPath);
    if (inFile.is_open()) {
        inFile >> index;
        inFile.close();
    } else {
        index = 0xFFFFFFFF;
        saveMetaData();
    }
}

std::string StorageManager::readRecord(uint32_t id) {

    if (buffer->contains(id)) {
        auto data = buffer->readData(id).getData();
        return std::to_string(data.first) + " - " + data.second;
    }

    auto [file_id, offset] = tree->search(id);
    std::string fileName = getFilePathByIndex(file_id);

    std::ifstream inFile(fileName, std::ios::binary);
    DataNode dataNode;

    if (!inFile) return "No Records Found";

    inFile.seekg(offset);

    if (inFile.read(reinterpret_cast<char *>(&dataNode), sizeof(DataNode))) {
        auto data = dataNode.getData();
        return std::to_string(data.first) + " - " + data.second;
    }
    return "No Records Found";
}

void StorageManager::overWriteRecord(uint32_t file_id, uint64_t offset, DataNode &node) {
    std::string fileName = getFilePathByIndex(file_id);
    std::fstream file(fileName, std::ios::binary | std::ios::in | std::ios::out);

    if (!file.is_open()) std::cerr << "\033[33mERROR: Unable to open the file " << fileName << "\033[0m" << std::endl;

    file.seekp(offset, std::ios::beg);
    file.write(reinterpret_cast<const char *>(&node), sizeof(DataNode));
    file.flush();
    file.close();
}

void StorageManager::writeRecord(std::ifstream &file) {
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

        if (buffer -> contains(id) || (tree -> search(id).file_id != 0xFFFFFFFF)) {
            std::cerr << "\033[33mWARNING: Duplicate ID found, Hence Ignored.\033[0m" << std::endl;
            continue;
        }

        buffer -> writeData(id, dataNode, sizeof(dataNode));

        if (buffer -> isFull()) buffer -> flush();
    }
}

void StorageManager::writeRecord(uint32_t id, std::string msg) {
    char buf[length] = {0};

    if (msg.size() > length)  std::cerr << "\033[33mWARNING: The data size exceeds " << length << ", hence excess length is truncated.\033[0m" << std::endl;

    std::strncpy(buf, msg.c_str(), length);
    DataNode dataNode = DataNode(id, buf);
    wal -> writeWAL(dataNode);

    if (buffer -> contains(id) || (tree -> search(id).file_id != 0xFFFFFFFF)) {
        std::cerr << "\033[33mWARNING: Duplicate ID found, Hence Ignored.\033[0m" << std::endl;
        return;
    }
    buffer -> writeData(id, dataNode, sizeof(dataNode));
    if (buffer -> isFull()) buffer -> flush();
}

void StorageManager::writeRecord(std::vector<DataNode> walBuf) {
    for (auto &node : walBuf) {
        uint32_t id = node.getData().first;
        buffer -> writeData(id, node, sizeof(node));
        if (buffer -> isFull()) buffer -> flush();
    }
}

void StorageManager::updateRecord(uint32_t id, std::string msg) {
    char buf[length] = {0};

    if (msg.size() > length) std::cerr << "\033[33mWARNING: The data size exceeds " << length << ", hence excess length is truncated.\033[0m" << std::endl;

    std::strncpy(buf, msg.c_str(), length);
    DataNode dataNode = DataNode(id, buf);
    wal -> writeWAL(dataNode);

    if (buffer -> contains(id)) buffer -> writeData(id, dataNode, sizeof(dataNode));
    else {
        auto [file_id, offset] = tree -> search(id);

        if (file_id != 0xFFFFFFFF) overWriteRecord(file_id, offset, dataNode);
        else std::cerr << "\033[33mWARNING: ID not found!\033[0m" << std::endl;
    }
}
