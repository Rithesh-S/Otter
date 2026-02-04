#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include "./OFS/DataNode/DataNode.h"
#include "./OFS/Buffer/Buffer.h"
#include "./StorageManager/StorageManager.h"

using std::cin;
using std::cout;
using std::endl;

int main() {
    bool running = true;
    std::string command;
    
    StorageManager storageManager;

    std::cout << "--------------- Welcome to Otter DB ---------------" << std::endl;
    while(running) {
        std::cout << "ott_db> ";
        std::cin >> command; 

        if(command == "LOAD" || command == "load") {
            std::string fileName;

            std::cout << "Enter the CSV File name: ";
            std::cin >> fileName;
            std::ifstream file(fileName);

            if (!file.is_open()) throw std::runtime_error("\033[31mERROR: File not found!\033[0m");

            storageManager.writeRecord(file);
        } else if(command == "INSERT" || command == "insert") {
            uint32_t id;
            std::string data;

            std::cout << "Enter the ID: ";
            std::cin >> id;
            std::cout << "Enter the Data: \n";
            std::getline(std::cin >> std::ws, data);

            storageManager.writeRecord(id, data);
        } else if(command == "UPDATE" || command == "update") {
            uint32_t id;
            std::string data;
            
            std::cout << "Enter the ID: ";
            std::cin >> id;
            std::cout << "Enter the Data: \n";
            std::getline(std::cin >> std::ws, data);

            storageManager.updateRecord(id, data);
        } else if(command == "SEARCH" || command == "search") {
            uint32_t id;

            std::cout << "Enter the Search ID: ";
            std::cin >> id;

            std::cout << storageManager.readRecord(id) << std::endl;
        } else if(command == "delete" || command == "DELETE") {
            uint32_t id;

            std::cout << "Enter the Delete ID: ";
            std::cin >> id;

            storageManager.deleteRecord(id);
        } else if(command == "EXIT" || command == "exit") {
            std::cout << "Shutting Down gracefully...";
            running = false;
        } else {
            std::cerr << "\033[31mERROR: Invalid command!\033[0m" << std::endl;
        }
    }
    return 0;
}