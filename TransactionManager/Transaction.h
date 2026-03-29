#pragma once

#include <set>
#include <string>
#include <memory>
#include <cstdint>
#include "./data/TransactionStatus.h"
#include "./RecoveryManager/WAL.h"

class Transaction {
    private: 
        const std::string walBinPath = "./RecoveryManager/bin/WALLog.bin";
        
        uint32_t curr_tx_id;
        uint32_t prev_tx_id;

        std::unique_ptr<WAL> wal;
        
    public:
        Transaction();
        
        bool begin();
        bool commit();
        bool checkpoint();
        bool log(RecordPointer rp, TStatus action, DataNode prev, DataNode curr);
        
        void recovery(std::vector<std::pair<DataNode, RecordPointer>>& recoveryNodes);

        bool isWalBinFull();
        bool shouldRecovery();
        bool isTransactionActive();
};