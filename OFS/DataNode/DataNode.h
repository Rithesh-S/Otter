#pragma once

#include <iostream>
#include <cstdint>
#include <cstring>

#pragma pack(push, 1)

class DataNode {
    private:
        static const uint8_t TOT_SPACE = 124;
        
        uint32_t id;
        char data[TOT_SPACE] = {0};
        
    public:
        DataNode();
        
        DataNode(uint32_t id);
        DataNode(uint32_t id, char (&d) [TOT_SPACE]);

        bool isEmpty();
        uint32_t getId() const;
        std::pair<uint32_t, std::string> getData();
};

#pragma pack(pop)

static_assert(sizeof(DataNode) == 128, "DataNode size must be exactly 128 bytes!");