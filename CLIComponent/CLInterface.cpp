#include "CLInterface.h"
#include "../StorageManager/StorageManager.h"

CLInterface::CLInterface(StorageManager* sm) : running(true), sm(sm) {}

void CLInterface::printHelp() {
    std::cout << "\n--- Otter DB Help Menu ---\n"
            << "+--------------------+-----------------------------------+\n" 
            << "| INSERT <id> <msg>; | Add a new record                  |\n"
            << "+--------------------+-----------------------------------+\n" 
            << "| SEARCH <id>;       | Fetch a record                    |\n"
            << "+--------------------+-----------------------------------+\n" 
            << "| UPDATE <id> <msg>; | Modify a record                   |\n"
            << "+--------------------+-----------------------------------+\n" 
            << "| DELETE <id>;       | Remove a record                   |\n"
            << "+--------------------+-----------------------------------+\n" 
            << "| EXIT               | Close the database                |\n"
            << "+--------------------+-----------------------------------+\n" 
            << "| *msg - do not exceeds 124 character, else truncated!   |\n"
            << "+--------------------------------------------------------+\n" 
            << std::endl;
}

void CLInterface::printPrompt() { std::cout << "\033[36;1mAceDB> \033[0m"; }

void CLInterface::start() {
    std::cout << "\033[2J\033[1;1H";
    std::cout << "Welcome to Otter DB Engine v1.0" << std::endl;
    std::cout << "Type 'HELP' for instructions." << std::endl;

    std::string input;
    while (running) {
        printPrompt();
        if (!std::getline(std::cin, input)) break;

        if (input.empty()) continue;
        processCommand(input);
    }
}

void CLInterface::processCommand(std::string &cmd) {
    std::string upperCmd = cmd;
    for (auto& c : upperCmd) c = toupper(c);

    if (upperCmd == "EXIT" || upperCmd == "QUIT") {
        std::cout << "\033[32mSUCCESS: Shutting Down Gracefully...\033[0m" << std::endl;
        running = false;
    }
    else if (upperCmd == "HELP") printHelp();
    else {
        Lexer lexer(cmd);
        lexer.tokenize();
        Parser parser(lexer.getTokens());
        parser.parse();
    }


    // else if (command == "INSERT") {
    //     uint32_t id;
    //     if (!(ss >> id)) {
    //         std::cout << "Usage: INSERT <id> <message>" << std::endl;
    //         return;
    //     }

    //     std::string msg;
    //     std::getline(ss >> std::ws, msg); 

    //     if (!msg.empty() && (msg.front() == '"' || msg.front() == '\'')) {
    //         msg.erase(0, 1);
    //         if (!msg.empty() && (msg.back() == '"' || msg.back() == '\'')) msg.pop_back();
    //     }
        
    //     sm->writeRecord(id, msg);
    //     std::cout << "OK. Record " << id << " inserted." << std::endl;
    // } else if (command == "SEARCH") {
    //     uint32_t id;
    //     if (ss >> id) std::cout << sm->readRecord(id) << std::endl;
    // } else if (command == "DELETE") {
    //     uint32_t id;
    //     if (ss >> id) {
    //         sm->deleteRecord(id);
    //         std::cout << "OK. Record " << id << " deleted." << std::endl;
    //     }
    // } else if (command == "UPDATE") {
    //     uint32_t id;
    //     if (!(ss >> id)) {
    //         std::cout << "Usage: UPDATE <id> <message>" << std::endl;
    //         return;
    //     }

    //     std::string msg;
    //     std::getline(ss >> std::ws, msg); 

    //     if (!msg.empty() && (msg.front() == '"' || msg.front() == '\'')) {
    //         msg.erase(0, 1);
    //         if (!msg.empty() && (msg.back() == '"' || msg.back() == '\'')) msg.pop_back();
    //     }

    //     sm->updateRecord(id, msg);
    //     std::cout << "OK. Record " << id << " updated." << std::endl;
    // } else std::cout << "\033[31mERROR: Unknown command '" << command << "'\033[0m" << std::endl;
}