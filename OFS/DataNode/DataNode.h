#pragma once

#include <iostream>
#include <iterator>
#include <cstdint>

class DataNode {

    private:
        uint32_t id;

        static const size_t tot_space = 120;

        char data[tot_space] = {0};

    public:
        DataNode() = default;

        void addData(char (&d) [120]);
        void getData(char* d);

};