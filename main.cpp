#include <iostream>
#include "./OFS/DataNode/DataNode.h"

using std::cout;
using std::cin;
using std::endl;

int main() {
    char d[120] = {0};
    char t[120] = {0};
    cout << "Enter the data:" << endl;
    cin >> d;

    DataNode dataNode;
    dataNode.addData(d);
    
    dataNode.getData(t);
    cout << t << endl;

    return 0;
}