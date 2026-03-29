#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include "../StorageManager/StorageManager.h"

int main() {
    StorageManager sm;

    const int total_records = 100000;

    std::cout << "--- Otter DB Performance Benchmark ---" << std::endl;
    std::cout << "Target: " << total_records << " records." << std::endl;

    // --- 1. INSERTION PHASE ---
    auto start_insert = std::chrono::high_resolution_clock::now();
    
    int insert_errors = 0;
    for (int i = 1; i <= total_records; i++) {
        std::string data = "Record_Payload_" + std::to_string(i);
        if (!sm.writeRecord(i, data)) {
            insert_errors++;
        }
        
        // Progress indicator for every 10k records
        if (i % 10000 == 0) std::cout << "Inserted " << i << " records..." << std::endl;
    }
    
    auto end_insert = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> insert_duration = end_insert - start_insert;

    // --- 2. VERIFICATION PHASE (SEARCH) ---
    std::cout << "\nStarting Integrity Audit..." << std::endl;
    auto start_search = std::chrono::high_resolution_clock::now();
    
    int search_errors = 0;
    for (int i = 1; i <= total_records; i++) {
        auto res = sm.readRecord(i);
        std::string expected = "Record_Payload_" + std::to_string(i);
        
        if (res.first == "" || res.second != expected) {
            search_errors++;
        }
    }
    
    auto end_search = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> search_duration = end_search - start_search;

    // --- 3. RESULTS ---
    std::cout << "\n==========================================" << std::endl;
    std::cout << "BENCHMARK RESULTS" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "Total Insertion Time : " << insert_duration.count() << " seconds" << std::endl;
    std::cout << "Avg Write Latency    : " << (insert_duration.count() / total_records) * 1000 << " ms/record" << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Total Search Time    : " << search_duration.count() << " seconds" << std::endl;
    std::cout << "Avg Read Latency     : " << (search_duration.count() / total_records) * 1000 << " ms/record" << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Insertion Errors     : " << insert_errors << std::endl;
    std::cout << "Verification Errors  : " << search_errors << std::endl;
    
    if (search_errors == 0 && insert_errors == 0) {
        std::cout << "\033[32mFINAL STATUS: DATABASE INTEGRITY VERIFIED (100%)\033[0m" << std::endl;
    } else {
        std::cout << "\033[31mFINAL STATUS: CORRUPTION DETECTED!\033[0m" << std::endl;
    }
    std::cout << "==========================================" << std::endl;

    return 0;
}