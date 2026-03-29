#include "StorageManager.h"

StorageManager::StorageManager() { 
    try {
        // if (transaction == nullptr) transaction = std::make_unique<Transaction>();
        if (lruCache == nullptr) lruCache = std::make_unique<LRU>();
        if (tree == nullptr) tree = std::make_unique<BTree>();
        if (bPool == nullptr) bPool = std::make_unique<BufferPool>(this);
        if (iQueue == nullptr) iQueue = std::make_unique<InsertionQueue>(this);
    } catch(const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "\033[31mERROR: Unexpected Error during Constructor Initialization.\033[0m" << std::endl;
        exit(1);
    }
}

// void StorageManager::init() {
//     if(transaction -> shouldRecovery()) {
//         std::vector<std::pair<DataNode, RecordPointer>> recoveryNodes;
//         transaction -> recovery(recoveryNodes);
//         // over write the records.
//         // using the recordpointer the page is overwritern.
//         // calls the btree to overwrite.
//     }
// }

std::pair<uint16_t, uint16_t> StorageManager::getNewIndexForBinFlush() { return lruCache -> getNewIndex(); }

std::fstream* StorageManager::getFileByIndex(uint32_t index) { 
    try {
        std::fstream* file = lruCache -> getFileFromLRU(index); 
        return file;
    } catch(const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return nullptr;
    }
}

std::pair<RecordPointer, std::fstream*> StorageManager::getInsertionPosAndFile() {
    RecordPointer rp = iQueue -> getRecordPointer();
    auto file = getFileByIndex(rp.file_id);
    if(file == nullptr) {
        throw std::runtime_error("\033[31mERROR: File Not Available.\033[0m");       
    }
    return { rp, file }; 
}

void StorageManager::writePageIntoBin(BufferFrame& frame) {
    if(!frame.isDirty()) return;
    auto [ file_id, page_no ] = frame.getFileIdAndPageNo();

    std::fstream* file = getFileByIndex(file_id);
    size_t pos = page_no * PAGE_SIZE;

    file -> clear();
    file -> seekp(pos, std::ios::beg);
    if(!file -> write(reinterpret_cast<const char*>(frame.getPage()), sizeof(Page))) {
        throw std::runtime_error("\033[31mERROR: Unable to Write Bin File.\033[0m");
    }
    file -> flush();
}

void StorageManager::writePageIntoBin(uint16_t file_id, uint16_t page_no, Page& page) {
    std::fstream* file = getFileByIndex(file_id);
    size_t pos = page_no * PAGE_SIZE;

    file -> clear();
    file -> seekp(pos, std::ios::beg);
    if(!file -> write(reinterpret_cast<const char*>(&page), sizeof(Page))) {
        throw std::runtime_error("\033[31mERROR: Unable to Write Bin File.\033[0m");
    }
    file -> flush();
}

void StorageManager::readPageFromBin(uint16_t file_id, uint16_t page_no, Page& page) {
    std::fstream* file = getFileByIndex(file_id);
    size_t pos = page_no * PAGE_SIZE;

    file -> seekg(0, std::ios::end);
    size_t fileSize = file -> tellg();

    if (pos >= fileSize) {
        page = Page(page_no); 
        return;
    }

    file -> clear();
    file -> seekg(pos, std::ios::beg);
    
    if (!file -> read(reinterpret_cast<char*>(&page), sizeof(Page))) {
        throw std::runtime_error("\033[31mERROR: Physical Read Failed at Page " + std::to_string(page_no) + "\033[0m");
    }
}

std::pair<std::string, std::string> StorageManager::readRecord(uint32_t id) {
    RecordPointer rp;
    if (!tree -> search(id, rp)) return { "", "" };
    
    Page* page = bPool -> getPage(rp.file_id, rp.page_no);
    
    DataNode dataNode;
    if(!page -> readData(dataNode, id)) return { "", "" };
    
    auto data = dataNode.getData();
    return { std::to_string(data.first), data.second };
}

void StorageManager::overWriteRecord(uint32_t file_id, uint64_t offset, DataNode &node) {
    auto file = getFileByIndex(file_id);
    file -> clear();
    file -> seekp(offset, std::ios::beg);
    file -> write(reinterpret_cast<const char *>(&node), sizeof(DataNode));
    file -> flush();
}

bool StorageManager::writeRecord(uint32_t id, std::string msg) {
    char buf[DATA_LENGTH] = {0};
    if (msg.size() > DATA_LENGTH)  std::cerr << "\033[33mWARNING: The data size exceeds " << DATA_LENGTH << ", hence excess length is truncated.\033[0m" << std::endl;
    std::memcpy(buf, msg.c_str(), std::min((size_t) DATA_LENGTH, msg.size()));

    DataNode dataNode = DataNode(id, buf);
    RecordPointer rp = iQueue -> getRecordPointer();
    
    if (!tree -> insert(id, rp)) {
        // std::cerr << "\033[33mWARNING: Duplicate ID found, Hence Ignored.\033[0m" << std::endl;
        return false;
    }

    Page* page = bPool -> getPage(rp.file_id, rp.page_no);
    page -> writeData(dataNode);
    return true;
}

bool StorageManager::updateRecord(uint32_t id, std::string msg) {
    char buf[DATA_LENGTH] = {0};
    if (msg.size() > DATA_LENGTH) std::cerr << "\033[33mWARNING: The data size exceeds " << DATA_LENGTH << ", hence excess length is truncated.\033[0m" << std::endl;
    std::memcpy(buf, msg.c_str(), std::min((size_t) DATA_LENGTH, msg.size()));

    DataNode dataNode = DataNode(id, buf);
    RecordPointer rp;

    if(!tree -> search(id, rp)) {
        // std::cerr << "\033[33mWARNING: ID not found!\033[0m" << std::endl;
        return false;
    }
    
    Page* page = bPool -> getPage(rp.file_id, rp.page_no);
    if(!page -> writeData(dataNode)) return false;      // Duplicate or Page is full
    return true;
}

bool StorageManager::deleteRecord(uint32_t id) {
    RecordPointer rp;
    if(!tree -> markAsDeleted(id, rp)) return false;    // ID not found

    Page* page = bPool -> getPage(rp.file_id, rp.page_no);
    if(!page -> removeData(id)) return false;           // ID not found

    iQueue -> putRecordPointer(rp);
    return true;
}
