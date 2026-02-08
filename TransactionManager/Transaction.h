#pragma once

#include <cstdint>
#include "./data/TransactionStatus.h"

class Transaction {
    private: 
        static bool isActive;
        static TStatus status;

    public:
        Transaction() = default;
        ~Transaction() = default;

        bool begin();
        bool isFailed();
        void commit();

};