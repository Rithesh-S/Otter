#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include "./OFS/DataNode/DataNode.h"
#include "./OFS/Buffer/Buffer.h"

using std::cin;
using std::cout;
using std::endl;

void readRecord(uint32_t id ,std::string filename) {
    std::ifstream inFile(filename, std::ios::binary );
    DataNode dataNode;

    if (!inFile) return;

    size_t pos = id * sizeof(DataNode);
    inFile.seekg(pos);
    
    if(inFile.read(reinterpret_cast<char*>(&dataNode), sizeof(DataNode))) {
        auto ii = dataNode.getData();
        cout << ii.first << "-" << ii.second << endl;
    }
    return;
}

void insertData(std::ifstream file, uint32_t length, Buffer buffer) {
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
        std::cerr << "\033[33mWARNING: The data size exceeds 124, hence excess length is truncated.\033[0m" << endl;
        
        std::strncpy(buf, msg.c_str(), length);
        
        DataNode dataNode = DataNode(id, buf);

        buffer.writeData(id, dataNode, sizeof(dataNode));

        if (buffer.isFull()) buffer.flush();
    }
}

int main() {
    const std::string filename = "data.csv";
    const size_t length = 124;

    char t[124];

    std::ifstream file(filename);
    Buffer buffer;

    if (!file.is_open())
        throw std::runtime_error("\033[31mERROR: File not found!\033[0m");

    insertData(std::move(file), length, buffer);
    readRecord(2, "OFS/bin/chunk_file_0.bin");

    return 0;
}