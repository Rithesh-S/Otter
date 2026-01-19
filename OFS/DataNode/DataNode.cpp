#include "DataNode.h"

DataNode::DataNode(uint32_t id, char (&d) [124]) : id(id) {
    for (size_t i = 0; i < tot_space - 1; i++) data[i] = d[i];
    data[123] = '\0';
}

std::pair<uint32_t,std::string> DataNode::getData() {
    return { id, data };
}