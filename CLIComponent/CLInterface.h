#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include "../QueryProcessor/Lexer/Lexer.h"

class StorageManager;

class CLInterface {
    private:
        bool running;
        StorageManager* sm;

        void printPrompt();
        void printHelp();
        void processCommand(std::string &cmd);

    public:
        CLInterface(StorageManager* sm);
        ~CLInterface() = default;

        void start();
};