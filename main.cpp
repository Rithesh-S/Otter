#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include "./StorageManager/StorageManager.h"
#include "./CLIComponent/CLInterface.h"

#ifdef _WIN32
#include <windows.h>
#endif

void enableVirtualTerminal() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

int main(int argc, char* argv[]) {
    enableVirtualTerminal();
    if (argc == 1) {
        std::string cmd = "start cmd /k " + std::string(argv[0]) + " --cli";
        system(cmd.c_str());
        return 0;
    }

    if (std::string(argv[1]) == "--cli") {
        StorageManager sm;
        sm.recover();
        CLInterface cli(&sm);
        cli.start();
    }

    return 0;
}