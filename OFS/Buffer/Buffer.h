#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <cstdint>
#include "../DataNode/DataNode.h"

class Buffer {

    private:
        std::map<uint32_t, DataNode> records;
        static const size_t max_bytes = 1200;
        size_t used_bytes = 0;

    public:
        Buffer() = default;
        void writeData(uint32_t id, DataNode record, size_t size);
        std::map<uint32_t, DataNode> readData();
};