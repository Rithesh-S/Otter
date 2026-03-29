#include "WAL.h"

const std::string binPath = "";

WAL::WAL(std::string binPath) : binPath(binPath) {
    file.open(binPath, std::ios::binary | std::ios::in | std::ios::out);
    
    if(!file.is_open()) {
        std::ofstream creator(binPath, std::ios::binary);
        if(!creator) throw std::runtime_error("\033[31mERROR:Unable to create file: WALLog.bin.\033[0m");
        creator.close();
        
        file.open(binPath, std::ios::binary | std::ios::in | std::ios::out);
        init();
    }

    if((seekHeader().magic != magic)) {
        clearFile();
        throw std::runtime_error("\033[31mERROR:Inappropiate WAL File detected.\033[0m");
    }
    generateCRC32Table();
}

WAL::~WAL() {
    if(file.is_open()) {
        file.flush();
        file.close();
    }
}

bool WAL::isFull() { 
    size_t max_size = seekHeader().page_size;
    size_t current_size = getWalLogFileSize();
    return current_size == max_size; 
}

bool WAL::isEmpty() {
    bool empty = getWalLogFileSize() == sizeof(WALHeader) || getWalLogFileSize() == 0;
    return empty;
}

size_t WAL::getWalLogFileSize() {
    if (std::filesystem::exists(binPath)) {
        return std::filesystem::file_size(binPath);
    }
    return 0;
}

size_t WAL::logCountInWal() {
    size_t curr_wal_size = getWalLogFileSize();
    return curr_wal_size / 512;
}

void WAL::init() {
    WALHeader header;
    file.clear();
    file.seekp(0, std::ios::beg);

    if(!file.write(reinterpret_cast<const char*>(&header), sizeof(WALHeader))) {
        throw std::runtime_error("\033[31mERROR:Unable to Write Header to WAL File.\033[0m");
    }

    file.flush();
}

WALHeader WAL::seekHeader() {
    WALHeader header;
    file.clear();
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(&header), sizeof(WALHeader));
    return header;
}

void WAL::clearFile() {
    file.flush();              
    std::filesystem::resize_file(binPath, 0);   
    file.clear();              

    file.seekp(0);              
    file.seekg(0); 

    init();
}

void WAL::logData(uint32_t tx_id, RecordPointer rp, TStatus action, DataNode prev, DataNode curr) {
    LogEntry frame(
        tx_id,
        action,
        rp,
        prev,
        curr
    );

    size_t checkSumSize = sizeof(LogEntry) - sizeof(uint32_t);
    uint32_t crc = generateCRC(&frame, checkSumSize);
    frame.putCRC(crc);

    try {
        writeWAL(frame);
    } catch(const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void WAL::logTransaction(uint32_t tx_id, TStatus action) {
    LogEntry frame(
        tx_id,
        action
    );

    size_t checkSumSize = sizeof(LogEntry) - sizeof(uint32_t);
    uint32_t crc = generateCRC(&frame, checkSumSize);
    frame.putCRC(crc);

    try {
        writeWAL(frame);
    } catch(const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void WAL::writeWAL(LogEntry& log) {
    file.clear();
    file.seekp(0, std::ios::end);
    if(!file.write(reinterpret_cast<const char*>(&log), sizeof(LogEntry))) {
        throw std::runtime_error("\033[31mERROR:Unable to Write WAL File.\033[0m");
    }
    file.flush();
}

bool WAL::readWAL(std::vector<LogEntry>& entries) {
    file.clear();
    file.seekg(sizeof(WALHeader), std::ios::beg);
    LogEntry log;
    while(file.read(reinterpret_cast<char*>(&log), sizeof(LogEntry))) {
        if(!verifyCRC(log)) return false;
        entries.push_back(log);
    }
    return file.eof();
}

bool WAL::verifyCRC(LogEntry& log) {
    size_t checkSumSize = sizeof(LogEntry) - sizeof(uint32_t);

    uint32_t receivedCRC = log.crc;
    uint32_t actualCRC = generateCRC(&log, checkSumSize);

    return receivedCRC == actualCRC;
}

uint32_t WAL::generateCRC(const void* data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t* byte_data = reinterpret_cast<const uint8_t*>(data);

    for (size_t i = 0; i < length; i++) {
        uint8_t table_index = (crc ^ byte_data[i]) & 0xFF;
        crc = (crc >> 8) ^ crc32_table[table_index];
    }

    return crc ^ 0xFFFFFFFF; 
}

void WAL::generateCRC32Table() {
    uint32_t polynomial = key;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (uint32_t j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
        crc32_table[i] = crc;
    }
}


