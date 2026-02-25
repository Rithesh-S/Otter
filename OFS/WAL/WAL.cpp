#include "WAL.h"

WAL::WAL(std::string binPath) {
    if(walFrame == nullptr) walFrame = std::make_unique<WALFrame>();
    file.open(binPath, std::ios::binary | std::ios::in | std::ios::out);
    
    if(!file.is_open()) {
        std::ofstream creator(binPath, std::ios::binary);
        if(!creator) {
            throw std::runtime_error("\033[31mERROR:Unable to create file: WALFrame.bin.\033[0m");
            return;
        }
        creator.close();
        
        file.open(binPath, std::ios::binary | std::ios::in | std::ios::out);
        saveNodesIntoWALBin();
    }
    generateCRC32Table();
}

WAL::~WAL() {
    if(file.is_open()) {
        file.flush();
        file.close();
    }
}

bool WAL::isFull() { return walFrame -> record_count == MAX_RECORD_COUNT; }

void WAL::saveNodesIntoWALBin() { 
    file.clear();
    file.seekp(0, std::ios::beg);
    file.write(reinterpret_cast<const char*>(walFrame.get()), sizeof(WALFrame)); 
    file.flush();
}

void WAL::walFrameClearAndSave() {
    walFrame -> record_count = 0;
    walFrame -> crc = 0;
    for(size_t i = 0; i < 4096; i++) walFrame -> node[i] = '0';
    saveNodesIntoWALBin();
}

void WAL::writeWAL(DataNode& node) {
    if((walFrame -> magic != magic) || !verifyCRC())  {
        walFrameClearAndSave();
        throw std::runtime_error("\033[31mERROR:Inappropiate WAL File detected.\033[0m");
    }
    
    char* dest = &walFrame -> node[walFrame -> record_count * nodeSize];
    std::memcpy(dest, &node, nodeSize);
    walFrame -> record_count++;
    walFrame -> crc = generateCRC(walFrame -> node, walFrame -> record_count * nodeSize);
    saveNodesIntoWALBin();
}

std::vector<DataNode> WAL::readWAL() {
    file.clear();
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(walFrame.get()), sizeof(WALFrame));
    std::vector<DataNode> records;

    for(size_t i = 0; i < walFrame -> record_count; i++) {
        char* dest = &walFrame -> node[i * nodeSize];
        DataNode* node = reinterpret_cast<DataNode*>(dest);
        auto [id, data] = node -> getData();
        records.push_back(*node);
    }
    return records;
}

bool WAL::verifyCRC() {
    uint32_t receivedCRC = walFrame -> crc;
    uint32_t actualCRC = generateCRC(walFrame -> node, walFrame -> record_count * nodeSize);
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


