#include "DataNode.h"

DataNode::DataNode() { std::memset(data, 0, TOT_SPACE); }

DataNode::DataNode(uint32_t id, char (&d) [TOT_SPACE]) : id(id) {
    std::memset(data, 0, TOT_SPACE);
    std::memcpy(data, d, TOT_SPACE);
}

DataNode::DataNode(uint32_t id) : id(id) {
    std::memset(data, 0, TOT_SPACE);
}

std::pair<uint32_t,std::string> DataNode::getData() {
    size_t actualLength = 0;
    while (actualLength < TOT_SPACE && data[actualLength] != '\0') {
        actualLength++;
    }

    return { id, std::string(data, actualLength) };
}