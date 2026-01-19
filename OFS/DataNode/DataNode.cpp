#include "DataNode.h"

void DataNode::addData(char (&d)[120]) {
    for(size_t i = 0; i < tot_space - 1; i++) data[i] = d[i];
    data[119] = '\0';
}

void DataNode::getData(char (&d)[120]) {
    std::memcpy(d, data, 120);
    d[119] = '\0';
}