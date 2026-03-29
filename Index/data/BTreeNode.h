#pragma once

#include <cstdint>
#include <iostream>
#include <cstring>
#include "./KeyMeta.h"

#define M 292

#pragma pack(push, 1)

struct BTNode {

    uint32_t n;                             // Number of children
    bool is_leaf;                           // Is the Node is leaf
    uint32_t keys[M - 1];                   // Record ID
    KeyMeta keyMeta[M - 1];                 // MetaData of ID
    uint32_t child_page[M];                 // TreeNode ID
    uint8_t padding[13];                     


    BTNode() {
        std::memset(this, 0, sizeof(BTNode));
        n = 0;
        is_leaf = true;
    }
};
#pragma pack(pop)

static_assert(sizeof(BTNode) == 4096, "BTNode size must be exactly 4096 bytes!");