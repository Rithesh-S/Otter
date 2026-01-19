#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include "../DataNode/DataNode.h"

class Buffer
{

private:
    std::map<uint32_t, DataNode> records;
    static const size_t max_bytes = 1024;
    size_t used_bytes = 0;

    bool saveTheNodesIntoBin(const std::string filename, std::map<uint32_t, DataNode> records);

public:
    Buffer() = default;
    ~Buffer() = default;

    void writeData(uint32_t id, DataNode record, size_t size);
    void flush();
    bool isFull();
    std::map<uint32_t, DataNode> readData();
};