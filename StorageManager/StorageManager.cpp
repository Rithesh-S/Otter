#include "StorageManager.h"

const size_t StorageManager::cacheSize = 50;
const uint8_t StorageManager::length = 124;

StorageManager::StorageManager() { 
    try {
        if (wal == nullptr) wal = std::make_unique<WAL>(walBinPath);
        if (transaction == nullptr) transaction = std::make_unique<Transaction>();
        if (tree == nullptr) tree = std::make_unique<BTree>(this, treeIndexPath);
        if (buffer == nullptr) buffer = std::make_unique<Buffer>(this, tree.get(), transaction.get());
        if (lruCache == nullptr) lruCache = std::make_unique<LRU>(this, cacheSize);
        if (iQueue == nullptr) iQueue = std::make_unique<InsertionQueue>(this, iQueueBinPath);
    } catch(...) {
        std::cerr << "\033[31mERROR:Unexpected Error during Constructor Initialization.\033[0m" << std::endl;
        exit(1);
    }
}

StorageManager::~StorageManager() { 
    saveMetaData();
    metaFile.close();
}

void StorageManager::init() {
    std::vector<DataNode> recoveryRecords = wal -> readWAL();
    buffer -> writeRecordsFromWal(recoveryRecords);

    if(wal -> isFull() || transaction -> isFailed()) {
        transaction -> commit();
        buffer -> flush();
        wal -> walFrameClearAndSave();
    }  
    loadMetaData();
}

std::string StorageManager::getBTreeIndexPath() { return treeIndexPath; }

std::string StorageManager::getWALBinPath() { return walBinPath; }

std::string StorageManager::getInsertionQueueBinPath() { return iQueueBinPath; }

uint32_t StorageManager::getCurrentBinIndex() { return index; }

void StorageManager::walFrameClearAndSave() { wal -> walFrameClearAndSave(); }

std::fstream* StorageManager::getFileByIndex(uint32_t index) { 
    try {
        std::fstream* file = lruCache -> getFileFromLRU(index); 
        return file;
    } catch(const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return nullptr;
    }
}

std::string StorageManager::getFilePathByIndex(uint32_t index) {
    std::string binFileName = basepath + "/Buffer/bin/chunk_file_" + std::to_string(index) + ".bin";
    return binFileName;
}

std::pair<RecordPointer, std::fstream*> StorageManager::getInsertionPosAndFile() {
    RecordPointer rp = iQueue -> getRecordPointer();
    auto file = getFileByIndex(rp.file_id);
    if(file == nullptr) {
        std::cerr << "\033[31mERROR:File Not Available.\033[0m" << std::endl;
        exit(1);        //program exit!
    }
    return { rp, file }; 
}

uint32_t StorageManager::getNewIndexForBinFlush() {
    index++;
    saveMetaData();
    return index;
}

void StorageManager::saveMetaData() {
    if (metaFile.is_open()) {
        metaFile.clear();
        metaFile.seekp(0, std::ios::beg);
        
        metaFile.write(reinterpret_cast<const char*>(&index), sizeof(index));
        metaFile.flush();
    }
}

void StorageManager::loadMetaData() {
    metaFile.open(metaDataPath, std::ios::binary | std::ios::in | std::ios::out);
    if(!metaFile.is_open()) {
        std::ofstream creator(metaDataPath, std::ios::binary);
        if(!creator) {
            throw std::runtime_error("\033[31mERROR:Unable to create file:" + metaDataPath + ".\033[0m");
            return;
        }
        creator.close();

        metaFile.open(metaDataPath, std::ios::binary | std::ios::in | std::ios::out);
        index = 0xFFFFFFFF;
        saveMetaData();
    }

    metaFile.clear();
    metaFile.seekg(0, std::ios::beg);
    metaFile.read(reinterpret_cast<char*>(&index), sizeof(index));

    if (metaFile.gcount() < sizeof(index)) {
        index = 0xFFFFFFFF;
        saveMetaData();
    }
}

std::pair<std::string, std::string> StorageManager::readRecord(uint32_t id) {
    if (buffer -> contains(id)) {
        auto data = buffer -> readData(id).getData();
        return { std::to_string(data.first), data.second };
    }
    
    auto [file_id, offset] = tree -> search(id);
    if (file_id == 0xFFFFFFFF) return { "", "" };
    
    auto file = getFileByIndex(file_id);
    DataNode dataNode;
    
    file -> clear();
    file -> seekg(offset);
    if (file -> read(reinterpret_cast<char *>(&dataNode), sizeof(DataNode))) {
        auto data = dataNode.getData();
        return { std::to_string(data.first), data.second };
    }
    return { "", "" };
}

void StorageManager::overWriteRecord(uint32_t file_id, uint64_t offset, DataNode &node) {
    auto file = getFileByIndex(file_id);
    file -> clear();
    file -> seekp(offset, std::ios::beg);
    file -> write(reinterpret_cast<const char *>(&node), sizeof(DataNode));
    file -> flush();
}

bool StorageManager::writeRecord(uint32_t id, std::string msg) {
    char buf[length] = {0};

    if (msg.size() > length)  std::cerr << "\033[33mWARNING: The data size exceeds " << length << ", hence excess length is truncated.\033[0m" << std::endl;

    std::memcpy(buf, msg.c_str(), std::min((size_t)length, msg.size()));
    DataNode dataNode = DataNode(id, buf);
    
    if (buffer -> contains(id) || (tree -> search(id).file_id != 0xFFFFFFFF)) {
        std::cerr << "\033[33mWARNING: Duplicate ID found, Hence Ignored.\033[0m" << std::endl;
        return false;
    }

    wal -> writeWAL(dataNode);  //wal
    buffer -> writeData(dataNode);
    if (buffer -> isFull()) buffer -> flush();
    return true;
}

bool StorageManager::updateRecord(uint32_t id, std::string msg) {
    char buf[length] = {0};

    if (msg.size() > length) std::cerr << "\033[33mWARNING: The data size exceeds " << length << ", hence excess length is truncated.\033[0m" << std::endl;

    std::memcpy(buf, msg.c_str(), std::min((size_t)length, msg.size()));
    DataNode dataNode = DataNode(id, buf);
    
    if (buffer -> contains(id)) {
        wal -> writeWAL(dataNode);  //wal
        buffer -> writeData(dataNode);
        return true;
    } else {
        auto [file_id, offset] = tree -> search(id);
        if (file_id != 0xFFFFFFFF) {
            wal -> writeWAL(dataNode);   //wal
            overWriteRecord(file_id, offset, dataNode);
            return true;
        } else {
            std::cerr << "\033[33mWARNING: ID not found!\033[0m" << std::endl;
            return false;
        }
    }
}

bool StorageManager::deleteRecord(uint32_t id) {
    if(buffer -> contains(id)) {
        DataNode node(id);
        wal -> writeWAL(node);   // wal
        buffer -> removeData(id);
    } else if(tree -> search(id).file_id != 0xFFFFFFFF) {
        RecordPointer rp = tree -> markAsDeleted(id);
        iQueue -> putRecordPointer(rp);
    } else {
        std::cerr << "\033[33mWARNING: ID not found!\033[0m" << std::endl;
        return false;
    }
    return true;
}
