#include "Transaction.h"

std::unique_ptr<WAL> wal = nullptr;

Transaction::Transaction() {
    try {
        if(wal == nullptr) wal = std::make_unique<WAL>(walBinPath);
    } catch(const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
    curr_tx_id = 0;
    prev_tx_id = curr_tx_id;
    begin();
}

bool Transaction::isTransactionActive() { return curr_tx_id != 0; }

bool Transaction::isWalBinFull() { return wal -> isFull(); };

bool Transaction::shouldRecovery() { return !wal -> isEmpty(); };

bool Transaction::begin() {
    if(curr_tx_id != 0) return false; 
    if(wal -> isFull()) {
        std::cerr << "\033[33mERROR:WAL Bin is full!\033[0m" << std::endl;
        return false;
    }

    uint32_t tx_id = prev_tx_id + 1;
    curr_tx_id = tx_id;
    prev_tx_id = curr_tx_id;

    wal -> logTransaction(tx_id, TStatus::BEGIN);
    return true;
}

bool Transaction::checkpoint() {
    if(curr_tx_id == 0) return false;
    if(wal -> isFull()) {
        std::cerr << "\033[33mERROR:WAL Bin is full!\033[0m" << std::endl;
        return false;
    }

    // flush the records into the tree and bin, by the bufferpool and call this function.
    
    wal -> logTransaction(curr_tx_id, TStatus::CHECKPOINT);
    return true;
}

bool Transaction::commit() {
    if(curr_tx_id == 0) return false;
    if(wal -> isFull()) {
        std::cerr << "\033[33mERROR:WAL Bin is full!\033[0m" << std::endl;
        return false;
    }
    
    // flush the records into the tree and bin, by the bufferpool and call this function.

    wal -> logTransaction(curr_tx_id, TStatus::COMMIT);
    prev_tx_id = curr_tx_id;
    curr_tx_id = 0;

    return true;
}

bool Transaction::log(RecordPointer rp, TStatus action, DataNode prev, DataNode curr) {
    if(wal -> isFull()) std::cerr << "\033[33mERROR:WAL Bin is full!\033[0m" << std::endl;

    bool auto_commit = (curr_tx_id == 0);
    if(auto_commit) begin();

    wal -> logData(curr_tx_id, rp, action, prev, curr);

    if(wal -> logCountInWal() % 1000 == 0) {
        checkpoint(); 
        return true;
    }
    return false;
}


void Transaction::recovery(std::vector<std::pair<DataNode, RecordPointer>>& recoveryNodes) {
    std::vector<LogEntry> entries;
    if(!wal -> readWAL(entries)) std::cerr << "\033[33mWARNING:Partial Recovery Initiated.\033[0m" << std::endl;

    std::set<uint32_t> commited_txs;
    size_t last_checkpoint_idx = 0;
    size_t entry_size = entries.size();

    for (int i = entry_size - 1; i >= 0; i--) {
        if (entries[i].action == TStatus::CHECKPOINT) {
            last_checkpoint_idx = i;
            break;
        }
    }
    
    for(int i = last_checkpoint_idx; i < entry_size; i++) {
        if(entries[i].action == TStatus::COMMIT) 
            commited_txs.insert(entries[i].tx_id);
    }

    for(int i = last_checkpoint_idx; i < entry_size; i++) {
        if(commited_txs.count(entries[i].tx_id)) {
            recoveryNodes.push_back({ entries[i].currRecord, entries[i].rp });
        }
    }

    wal -> clearFile();
}