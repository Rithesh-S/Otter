#pragma once

#include <memory>
#include "../../data/BTreeNode.h"

struct CachedNode {
    uint32_t page_id;
    std::shared_ptr<BTNode> node;

    CachedNode(uint32_t page_id, std::shared_ptr<BTNode> node) : page_id(page_id), node(node) {}
};

