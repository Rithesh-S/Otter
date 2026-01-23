#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include "./OFS/DataNode/DataNode.h"
#include "./OFS/Buffer/Buffer.h"
#include "./OFS/StorageManager/StorageManager.h"

using std::cin;
using std::cout;
using std::endl;

int main() {
    const std::string filename = "data.csv";
    const size_t length = 124;

    char t[124];

    std::ifstream file(filename);
    Buffer buffer;
    StorageManager storageManager;

    if (!file.is_open())
        throw std::runtime_error("\033[31mERROR: File not found!\033[0m");

    storageManager.writeRecord(std::move(file), length, buffer);
    cout << storageManager.readRecord(2);

    return 0;
}