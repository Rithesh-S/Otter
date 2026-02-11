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

void CLInterface::printFormat(std::pair<std::string, std::string> node, QueryType type) {
    if(type == QueryType::SEARCH && node.first == "" && node.second == "") {
        std::cout << "\033[33mNo Records Found!\033[0m" << std::endl;
        return;
    }

    std::string id = node.first;
    std::string msg = node.second;

    switch (type) {
        case QueryType::INSERT:
            std::cout << "\033[32mOK. Record " << id << " inserted.\033[0m" << std::endl;
            return;
        case QueryType::UPDATE:
            std::cout << "\033[32mOK. Record " << id << " updated.\033[0m" << std::endl;
            return;
        case QueryType::DELETE:
            std::cout << "\033[32mOK. Record " << id << " deleted.\033[0m" << std::endl;
            return;
        case QueryType::SEARCH:
            break;
        default:
            return;
    }

    const int idWidth = 10;
    const int msgWidth = 124;

    auto printLine = [&]() {
        std::cout << "+" << std::string(idWidth + 2, '-') 
                  << "+" << std::string(msgWidth + 2, '-') << "+\n";
    };

    printLine();
    std::cout << "| " << std::left << std::setw(idWidth) << "ID" 
              << " | " << std::left << std::setw(msgWidth) << "DATA CONTENT" << " |\n";
    
    printLine();
    std::cout << "| " << std::left << std::setw(idWidth) << id 
              << " | " << std::left << std::setw(msgWidth) << msg << " |\n";

    printLine();
    std::cout << std::endl;
}

void CLInterface::processCommand(std::string &cmd) {
    std::string upperCmd = cmd;
    for (auto& c : upperCmd) c = toupper(c);

    if (upperCmd == "EXIT" || upperCmd == "EXIT;") {
        std::cout << "\033[32mSUCCESS: Shutting Down Gracefully...\033[0m" << std::endl;
        running = false;
    }
    else if (upperCmd == "HELP" || upperCmd == "HELP;") printHelp();
    else {
        Lexer lexer(cmd);
        lexer.tokenize();

        Parser parser(lexer.getTokens());
        parser.parse();

        Executor executor(parser.getRoot(), sm, this);
        executor.execute();
    }
}