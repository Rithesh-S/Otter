#pragma once

#include <string>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include "../OFS/DataNode/DataNode.h"
#include "../QueryProcessor/Lexer/Lexer.h"
#include "../QueryProcessor/Parser/Parser.h"
#include "../QueryProcessor/Executor/Executor.h"
#include "../QueryProcessor/Executor/data/QueryPlan.h"

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
        void printFormat(std::pair<std::string, std::string> node, QueryType type);
};