#pragma once

#include <iostream>
#include <cstdint>
#include <cstring>

#pragma pack(push, 1)

class DataNode {

    private:
        static const uint8_t tot_space = 124;
        uint32_t id;
        char data[tot_space] = {0};
        
    public:
        DataNode() = default;
        ~DataNode() = default;
        
        DataNode(uint32_t id, char (&d) [tot_space]);

        std::pair<uint32_t, std::string> getData();
};

#pragma pack(pop)

static_assert(sizeof(DataNode) == 128, "DataNode size must be exactly 128 bytes!");