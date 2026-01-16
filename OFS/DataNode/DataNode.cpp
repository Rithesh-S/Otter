#include "DataNode.h"

void DataNode::addData(char (&d) [120]) {
    for(size_t i = 0; i < tot_space; i++) data[i] = d[i];
}

void DataNode::getData(char* d) {
    for(size_t i = 0; i < tot_space; i++) d[i] = data[i];
}