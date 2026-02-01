#include "DataNode.h"

DataNode::DataNode(uint32_t id, char (&d) [tot_space]) : id(id) {
    for (size_t i = 0; i < tot_space - 1; i++) data[i] = d[i];
    data[tot_space - 1] = '\0';
}

std::pair<uint32_t,std::string> DataNode::getData() {
    return { id, data };
}