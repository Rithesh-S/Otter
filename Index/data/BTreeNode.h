#pragma once

#include <cstdint>
#include <iostream>
#include <cstring>
#include "./RecordPointer.h"

#define M 204

#pragma pack(push, 1)

struct BTNode {

    uint32_t n;                             //number of children
    bool is_leaf;                           //denotes the node is leaf or not
    uint32_t keys[M - 1];                   //the record ids
    RecordPointer recordPointer[M - 1];     //position of the record in file
    uint32_t child_page[M];                 //file id
    uint8_t padding[27];                    //fills the remaining space to fill the 4096 bytes


    BTNode() {
        n = 0;
        is_leaf = true;
        std::memset(this, 0, sizeof(BTNode));
    }
    ~BTNode() = default;
};
#pragma pack(pop)

static_assert(sizeof(BTNode) == 4096, "BTNode size must be exactly 4096 bytes!");