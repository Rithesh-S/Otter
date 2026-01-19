#pragma once

#include <iostream>
#include <iterator>
#include <cstdint>
#include <cstring>

class DataNode {

    private:
        static const size_t tot_space = 120;
        uint32_t id;
        char data[tot_space] = {0};

    public:
        DataNode() = default;

        void addData(char (&d)[120]);
        void getData(char (&d)[120]);

};