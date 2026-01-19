#pragma once

#include <iostream>
#include <iterator>
#include <cstdint>
#include <cstring>

class DataNode {

    private:
        static const size_t tot_space = 124;
        uint32_t id;
        char data[tot_space] = {0};
        
    public:
        DataNode(uint32_t id, char (&d) [124]);
        DataNode() = default;
        ~DataNode() = default;

        std::pair<uint32_t, std::string> getData();
};