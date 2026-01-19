#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>
#include "./OFS/DataNode/DataNode.h"
#include "./OFS/Buffer/Buffer.h"

using std::cout;
using std::cin;
using std::endl;

int main() {
    const std::string filename = "data.csv";
    const size_t length = 120;
    
    std::string line;
    char t[120];
    
    std::ifstream file(filename);
    Buffer buffer;
    
    if(!file.is_open()) throw std::runtime_error("\033[31mERROR: File not found!\033[0m");

    while(std::getline(file, line)) {
        uint32_t id;
        std::string temp_id, msg;
        char buf[120] = {0};

        std::stringstream ss(line);
        DataNode dataNode;

        if(std::getline(ss, temp_id, ',')) id = std::stoull(temp_id);
        std::getline(ss, msg, ',');

        if(msg.size() > 120) std::cerr << "\033[33mWARNING: The data size exceeds 120, hence excess length is truncated.\033[0m" << endl;

        std::strncpy(buf, msg.c_str(), length);

        dataNode.addData(buf);

        buffer.writeData(id, dataNode, length);
        
    }

    std::map<uint32_t, DataNode> records = buffer.readData();
    for(auto i:records) {
        i.second.getData(t);
        cout << i.first << "-" << t << endl;
    }

    return 0;
}